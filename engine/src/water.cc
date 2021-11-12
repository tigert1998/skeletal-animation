#include "water.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "vertex.h"

Water::Water(int height, int width, float length)
    : height_(height),
      width_(width),
      u_((height + 1) * (width + 1)),
      v_((height + 1) * (width + 1)),
      buf_((height + 1) * (width + 1)),
      length_(length),
      vertices_((height + 1) * (width + 1)) {
  shader_.reset(new Shader(kVsSource, kFsSource));

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  std::vector<uint32_t> indices;
  for (int i = 0; i < height_; i++)
    for (int j = 0; j < width_; j++) {
      int a = Index(i, j);
      int b = Index(i, j + 1);
      int c = Index(i + 1, j);
      int d = Index(i + 1, j + 1);
      indices.push_back(a);
      indices.push_back(b);
      indices.push_back(c);
      indices.push_back(b);
      indices.push_back(c);
      indices.push_back(d);
    }

  indices_size_ = indices.size();

  for (int i = 0; i <= height_; i++)
    for (int j = 0; j <= width_; j++) {
      int idx = Index(i, j);
      u_[idx] = 0;
      v_[idx] = 0;
    }

  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices_size_,
               indices.data(), GL_STATIC_DRAW);
  glBindVertexArray(0);
}

Water::~Water() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
}

int Water::Index(int x, int y) { return x * (width_ + 1) + y; }

float Water::GetU(int x, int y) {
  if (x < 0) x = 0;
  if (x > height_) x = height_;
  if (y < 0) y = 0;
  if (y > width_) y = width_;
  auto idx = Index(x, y);
  return u_[idx];
}

void Water::StepSimulation(double delta_time) {
  const float c = 4;

  for (int i = 0; i <= height_; i++) {
    for (int j = 0; j <= width_; j++) {
      int idx = Index(i, j);
      float tmp =
          (GetU(i - 1, j) + GetU(i + 1, j) + GetU(i, j - 1) + GetU(i, j + 1)) *
              0.25 -
          GetU(i, j);
      v_[idx] += c * tmp * delta_time;
      v_[idx] *= 0.99;
      buf_[idx] = GetU(i, j) + v_[idx];
    }
  }

  u_ = buf_;
}

void Water::Draw(Camera *camera, LightSources *light_sources) {
  for (int i = 0; i <= height_; i++)
    for (int j = 0; j <= width_; j++) {
      float x = length_ * i / height_;
      float y = GetU(i, j);
      float z = length_ * j / width_;
      auto position = glm::vec3(x, y, z);
      float dydx = (GetU(i + 1, j) - GetU(i - 1, j)) / 2 / (length_ / height_);
      float dydz = (GetU(i, j + 1) - GetU(i, j - 1)) / 2 / (length_ / width_);
      auto normal = glm::normalize(glm::vec3(-dydx, 1, -dydz));
      vertices_[Index(i, j)] = VertexType{position, normal};
    }

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * vertices_.size(),
               vertices_.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType),
                        (void *)offsetof(VertexType, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(VertexType),
                        (void *)offsetof(VertexType, normal));

  shader_->Use();
  shader_->SetUniform<glm::mat4>("uModelMatrix", glm::mat4(1));
  shader_->SetUniform<glm::mat4>("uViewMatrix", camera->view_matrix());
  shader_->SetUniform<glm::mat4>("uProjectionMatrix",
                                 camera->projection_matrix());
  light_sources->Set(shader_.get());

  glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

std::string Water::kVsSource = R"(
#version 410 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vPosition;
out vec3 vNormal;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
    vPosition = vec3(uModelMatrix * vec4(aPosition, 1));
    vNormal = aNormal;
}
)";

std::string Water::kFsSource = R"(
#version 410 core

in vec3 vPosition;
in vec3 vNormal;
)" + LightSources::kFsSource + R"(
out vec4 fragColor;

vec3 calcDiffuse(vec3 raw) {
    vec3 normal = normalize(vNormal);
    vec3 ans = vec3(0);
    for (int i = 0; i < uDirectionalLightCount; i++) {
        vec3 dir = normalize(-uDirectionalLights[i].dir);
        ans += max(dot(normal, dir), 0.0) * uDirectionalLights[i].color;
    }
    for (int i = 0; i < uPointLightCount; i++) {
        vec3 dir = normalize(uPointLights[i].pos - vPosition);
        ans += max(dot(normal, dir), 0.0) * uPointLights[i].color;
    }
    return ans * raw;
}

vec3 defaultShading() {
    vec3 color = vec3(0.4549f, 0.6470f, 0.7686f);
    return color * 0.2 + calcDiffuse(color);
}

void main() {
    fragColor = vec4(defaultShading(), 1);
}
)";