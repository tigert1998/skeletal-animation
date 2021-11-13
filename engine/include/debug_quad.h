#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "shader.h"

class DebugQuad {
 private:
  static std::string kVsSource, kGsSource, kFsSource;
  static std::shared_ptr<Shader> kShader;
  static uint32_t vao_;

  std::shared_ptr<Shader> shader_;

 public:
  DebugQuad();
  void Draw(uint32_t texture_id, glm::vec4 location);
};