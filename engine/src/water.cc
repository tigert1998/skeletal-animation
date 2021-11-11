#include "water.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "vertex.h"

Water::Water(int height, int width, float length)
    : height_(height),
      width_(width),
      u_(height * width),
      v_(height * width),
      buf_(height * width),
      length_(length) {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
}

Water::~Water() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

int Water::Index(int x, int y) { return x * width_ + y; }

float Water::GetU(int x, int y) {
  if (x < 0) x = 0;
  if (x >= height_) x = height_ - 1;
  if (y < 0) y = 0;
  if (y >= width_) y = width_ - 1;
  auto idx = Index(x, y);
  return u_[idx];
}

void Water::StepSimulation(double delta_time) {
  const float c = 16;

  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
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

void Water::Draw(Camera *camera) {
  using WaterVertex = Vertex<0, false>;
  std::vector<WaterVertex> vertices;

  std::vector<WaterVertex> vertices;
  for (int i = 0; i < height_; i++)
    for (int j = 0; j < width_; j++) {
      float x = length_ * i / height_;
      float y = GetU(i, j);
      float z = length_ * j / width_;
      vertices.push_back(WaterVertex{glm::vec3(x, y, z), glm::vec3(0)});
    }

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
}