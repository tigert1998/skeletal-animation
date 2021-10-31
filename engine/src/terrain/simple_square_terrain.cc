#include "terrain/simple_square_terrain.h"

#include <glad/glad.h>

#include <vector>

#include "texture_manager.h"
#include "vertex.h"

const std::string SimpleSquareTerrain::kVsSource = R"(
#version 410 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
    vPosition = vec3(uModelMatrix * vec4(aPosition, 1));
    vTexCoord = aTexCoord;
    vNormal = aNormal;
}
)";

const std::string SimpleSquareTerrain::kFsSource = R"(
#version 410 core

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
)" + LightSources::kFsSource + R"(
out vec4 fragColor;

uniform float uLength;
uniform sampler2D uTerrainTexture;

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
    vec3 color = vec3(0.9608f, 0.6784f, 0.2588f);
    return color * 0.2 + calcDiffuse(color);
}

void main() {
    fragColor = vec4(calcDiffuse(texture(uTerrainTexture, vTexCoord * uLength).rgb), 1);
}
)";

SimpleSquareTerrain::SimpleSquareTerrain(int size, double length,
                                         const std::string &texture_path)
    : size_(size), length_(length), ratio_(0.25) {
  // size * size squares
  perlin_noise_.reset(new PerlinNoise(1024, 10086));
  shader_.reset(new Shader(Shader::SRC, kVsSource, kFsSource));
  texture_id_ = TextureManager::LoadTexture(texture_path, GL_REPEAT);

  std::vector<Vertex<0>> vertices;
  vertices.reserve((size + 1) * (size + 1));
  for (int i = 0; i <= size; i++)
    for (int j = 0; j <= size; j++) {
      double x = 1.0 * i / size;
      double z = 1.0 * j / size;
      double y = get_height(x * length_, z * length_);
      auto normal = get_normal(x * length_, z * length_);
      Vertex<0> vertex;
      vertex.position = glm::vec3(x * length_, y, z * length_);
      vertex.tex_coord = glm::vec2(x, z);
      vertex.normal = normal;
      vertices.push_back(vertex);
    }

  std::vector<int> indices;
  indices.reserve(size * size * 6);
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++) {
      int a = i * (size + 1) + j;
      int b = i * (size + 1) + j + 1;
      int c = (i + 1) * (size + 1) + j;
      int d = (i + 1) * (size + 1) + j + 1;
      indices.push_back(a);
      indices.push_back(b);
      indices.push_back(c);
      indices.push_back(b);
      indices.push_back(c);
      indices.push_back(d);
    }

  indices_size_ = indices.size();

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex<0>) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex<0>),
                        (void *)offsetof(Vertex<0>, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex<0>),
                        (void *)offsetof(Vertex<0>, tex_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex<0>),
                        (void *)offsetof(Vertex<0>, normal));

  glBindVertexArray(0);
}

double SimpleSquareTerrain::get_height(double x, double y) {
  return perlin_noise_->Noise(x * ratio_, y * ratio_, 0);
}

glm::vec3 SimpleSquareTerrain::get_normal(double x, double y) {
  auto vec = perlin_noise_->DerivativeNoise(x * ratio_, y * ratio_, 0);
  return glm::normalize(glm::vec3(-vec.x * ratio_, 1, -vec.y * ratio_));
}

void SimpleSquareTerrain::Draw(Camera *camera_ptr,
                               LightSources *light_sources) {
  shader_->Use();
  light_sources->Set(shader_.get());
  if (light_sources != nullptr) {
    light_sources->Set(shader_.get());
  }
  shader_->SetUniform<glm::mat4>("uModelMatrix", glm::mat4(1));
  shader_->SetUniform<glm::mat4>("uViewMatrix", camera_ptr->view_matrix());
  shader_->SetUniform<glm::mat4>("uProjectionMatrix",
                                 camera_ptr->projection_matrix());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  shader_->SetUniform<int32_t>("uTerrainTexture", 0);
  shader_->SetUniform<float>("uLength", length_);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}