#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>

#include "camera.h"
#include "light_sources.h"
#include "shader.h"
#include "terrain/perlin_noise.h"

class SimpleSquareTerrain {
 private:
  std::unique_ptr<PerlinNoise> perlin_noise_;
  int size_;
  double length_;
  double ratio_;
  uint32_t vao_, vbo_, ebo_, indices_size_, texture_id_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<btTriangleMesh> bt_triangle_mesh_;

  glm::vec3 get_normal(double x, double y);

  static const std::string kVsSource, kFsSource;

 public:
  SimpleSquareTerrain(int size, double length, const std::string &texture_path);

  double get_height(double x, double y);

  inline btTriangleMesh *get_bt_triangle_mesh() {
    return bt_triangle_mesh_.get();
  }

  void Draw(Camera *camera_ptr, LightSources *light_sources);
};