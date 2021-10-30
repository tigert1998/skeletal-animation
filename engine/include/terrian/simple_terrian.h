#pragma once

#include <memory>

#include "camera.h"
#include "light_sources.h"
#include "shader.h"
#include "terrian/perlin_noise.h"

class SimpleSquareTerrian {
 private:
  std::unique_ptr<PerlinNoise> perlin_noise_;
  int size_;
  uint32_t vao_, vbo_, ebo_, indices_size_;
  std::unique_ptr<Shader> shader_;

  double get_height(double x, double y);

  static const std::string kVsSource, kFsSource;

 public:
  SimpleSquareTerrian(int size);

  void Draw(Camera *camera_ptr, LightSources *light_sources);
};