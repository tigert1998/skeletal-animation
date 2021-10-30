#pragma once

#include <memory>

#include "camera.h"
#include "light_sources.h"
#include "terrian/perlin_noise.h"

class SimpleSquareTerrian {
 private:
  std::unique_ptr<PerlinNoise> perlin_noise_;
  int size_;

  double get_height(double x, double y);

 public:
  SimpleSquareTerrian(int size);

  void Draw(Camera *camera_ptr, LightSources *light_sources);
};