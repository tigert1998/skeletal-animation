#include "water.h"

#include <glad/glad.h>
#include <glog/logging.h>

#include <glm/glm.hpp>

#include "texture_manager.h"
#include "utils.h"
#include "vertex.h"

Water::Water(int height, int width, float height_length, int tex_height,
             int tex_width)
    : height_(height),
      width_(width),
      tex_height_(tex_height * FB_HW_RATIO),
      tex_width_(tex_width * FB_HW_RATIO),
      u_((height + 1) * (width + 1)),
      v_((height + 1) * (width + 1)),
      buf_((height + 1) * (width + 1)),
      height_length_(height_length),
      width_length_(width_ * height_length / height_),
      vertices_((height + 1) * (width + 1)) {
  shader_.reset(new Shader(kVsSource, kFsSource));

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  glGenFramebuffers(1, &reflection_fbo_);
  glGenFramebuffers(1, &refraction_fbo_);

  glBindFramebuffer(GL_FRAMEBUFFER, reflection_fbo_);
  reflection_tex_id_ =
      TextureManager::AllocateTexture(tex_height_, tex_width_, GL_RGB);
  reflection_rbo_ = TextureManager::AllocateRenderBuffer(
      tex_height_, tex_width_, GL_DEPTH24_STENCIL8);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         reflection_tex_id_, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, reflection_rbo_);
  CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  glBindFramebuffer(GL_FRAMEBUFFER, refraction_fbo_);
  refraction_tex_id_ =
      TextureManager::AllocateTexture(tex_height_, tex_width_, GL_RGB);
  refraction_rbo_ = TextureManager::AllocateRenderBuffer(
      tex_height_, tex_width_, GL_DEPTH24_STENCIL8);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         refraction_tex_id_, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, refraction_rbo_);
  CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
  glDeleteFramebuffers(1, &reflection_fbo_);
  glDeleteFramebuffers(1, &refraction_fbo_);
  glDeleteTextures(1, &reflection_tex_id_);
  glDeleteTextures(1, &refraction_tex_id_);
  glDeleteRenderbuffers(1, &reflection_rbo_);
  glDeleteRenderbuffers(1, &refraction_rbo_);
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

void Water::Draw(Camera *camera, LightSources *light_sources,
                 const std::function<void(Camera *, glm::vec4)> &render,
                 glm::mat4 model_matrix) {
  int viewport_args[4];
  glGetIntegerv(GL_VIEWPORT, viewport_args);

  auto position = camera->position();
  auto front = camera->front();

  glViewport(0, 0, tex_width_, tex_height_);
  glBindFramebuffer(GL_FRAMEBUFFER, reflection_fbo_);
  // the normal of the water surface must be (0, 1, 0)
  auto get_height = [](glm::mat4 model_matrix) -> float {
    auto vec4 = model_matrix * glm::vec4(0, 0, 0, 1);
    return (vec4 / vec4.w).y;
  };

  float height = get_height(model_matrix);
  camera->set_position(
      glm::vec3(position.x, 2 * height - position.y, position.z));

  camera->set_beta(-camera->beta());
  render(camera, glm::vec4(0, 1, 0, -height));

  camera->set_position(position);
  camera->set_front(front);
  glBindFramebuffer(GL_FRAMEBUFFER, refraction_fbo_);
  render(camera, glm::vec4(0, -1, 0, height));

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(viewport_args[0], viewport_args[1], viewport_args[2],
             viewport_args[3]);
  for (int i = 0; i <= height_; i++)
    for (int j = 0; j <= width_; j++) {
      float x = height_length_ * i / height_;
      float y = GetU(i, j);
      float z = width_length_ * j / width_;
      auto position = glm::vec3(x, y, z);
      float dydx =
          (GetU(i + 1, j) - GetU(i - 1, j)) / 2 / (height_length_ / height_);
      float dydz =
          (GetU(i, j + 1) - GetU(i, j - 1)) / 2 / (width_length_ / width_);
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
  shader_->SetUniform<glm::mat4>("uModelMatrix", model_matrix);
  shader_->SetUniform<glm::mat4>("uViewMatrix", camera->view_matrix());
  shader_->SetUniform<glm::mat4>("uProjectionMatrix",
                                 camera->projection_matrix());
  shader_->SetUniform<glm::vec3>("uCameraPosition", camera->position());
  // light_sources->Set(shader_.get());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, reflection_tex_id());
  shader_->SetUniform<int32_t>("uReflectionTexture", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, refraction_tex_id());
  shader_->SetUniform<int32_t>("uRefractionTexture", 1);

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
out vec4 vClipSpacePosition;

void main() {
    vClipSpacePosition = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1); 
    gl_Position = vClipSpacePosition;
    vPosition = vec3(uModelMatrix * vec4(aPosition, 1));
    vNormal = vec3(uModelMatrix * vec4(aNormal, 0));
}
)";

std::string Water::kFsSource = R"(
#version 410 core

in vec3 vPosition;
in vec3 vNormal;
in vec4 vClipSpacePosition;

)" + LightSources::kFsSource + R"(
out vec4 fragColor;

uniform sampler2D uReflectionTexture;
uniform sampler2D uRefractionTexture;
uniform vec3 uCameraPosition;

vec3 calcLights() {
    vec3 ans = vec3(0);
    vec3 viewDirection = uCameraPosition - vPosition;
    for (int i = 0; i < uDirectionalLightCount; i++) {
        vec3 specular = calcSpecular(-uDirectionalLights[i].dir, vNormal, viewDirection, 20, vec3(1));
        ans += specular * uDirectionalLights[i].color;
    }
    for (int i = 0; i < uPointLightCount; i++) {
        vec3 specular = calcSpecular(uPointLights[i].pos - vPosition, vNormal, viewDirection, 20, vec3(1));
        ans += specular * uPointLights[i].color;
    }
    return ans;
}

void main() {
    vec2 texCoord = ((vClipSpacePosition.xy / vClipSpacePosition.w) + 1) * 0.5;

    float refractiveFactor = dot(normalize(uCameraPosition - vPosition), vNormal);
    refractiveFactor = pow(refractiveFactor, 1);

    fragColor = mix(
        texture(uReflectionTexture, vec2(texCoord.x, 1 - texCoord.y)),
        texture(uRefractionTexture, texCoord),
        refractiveFactor
    );
    fragColor = mix(fragColor, vec4(0, 0.3, 0.5, 1), 0.2);
    fragColor += vec4(calcLights(), 0);
}
)";