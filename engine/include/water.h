#pragma once

#include <memory>
#include <vector>

#include "camera.h"
#include "light_sources.h"
#include "shader.h"
#include "vertex.h"

class Water {
 private:
  using VertexType = Vertex<0, false>;

  static std::string kVsSource, kFsSource;

  std::vector<float> u_, v_, buf_;
  int height_, width_, indices_size_;
  float height_length_, width_length_;
  uint32_t vao_, vbo_, ebo_;
  std::unique_ptr<Shader> shader_;

  std::vector<VertexType> vertices_;

  int Index(int x, int y);
  float GetU(int x, int y);

 public:
  Water(int height, int width, float height_length);
  ~Water();
  void StepSimulation(double delta_time);
  void Draw(Camera *camera, LightSources *light_sources);
};