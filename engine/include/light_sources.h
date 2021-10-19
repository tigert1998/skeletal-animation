#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "shader.h"

class Light {
 public:
  virtual void Set(Shader *shader) = 0;
};

class Directional : public Light {
 private:
  glm::vec3 dir_, color_;

 public:
  explicit Directional(glm::vec3 dir, glm::vec3 color);
  void Set(Shader *shader) override;
};

class Point : public Light {
 private:
  glm::vec3 pos_, color_;

 public:
  explicit Point(glm::vec3 pos, glm::vec3 color);
  void Set(Shader *shader) override;
};

class LightSources : public Light {
 private:
  std::vector<std::unique_ptr<Light>> lights_;

 public:
  void Add(std::unique_ptr<Light> light);
  void Set(Shader *shader) override;
};