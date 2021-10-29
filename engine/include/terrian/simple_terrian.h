#pragma once

#include <memory>

#include "camera.h"
#include "light_sources.h"
#include "terrian/perlin_noise.h"

class SimpleTerrian {
 private:
  std::unique_ptr<PerlinNoise> perlin_noise_;

 public:
  SimpleTerrian();

  void Draw(Camera *camera_ptr, LightSources *light_sources);
};