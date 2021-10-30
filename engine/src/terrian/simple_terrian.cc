#include "terrian/simple_terrian.h"

#include <vector>

#include "vertex.h"

SimpleSquareTerrian::SimpleSquareTerrian(int size) : size_(size) {
  perlin_noise_.reset(new PerlinNoise(512, 10086));

  std::vector<Vertex<0>> vertices;
  vertices.reserve(size * size);
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++) {
      double x = 1.0 * i / size;
      double z = 1.0 * j / size;
      double y = get_height(x, z);
      Vertex<0> vertex{
          .position = glm::vec3(x, 0, z),
          .tex_coord = glm::vec2(x, z),
          .normal = glm::vec3(0, 1, 0),
      };
      vertices.push_back(vertex);
    }
}

double SimpleSquareTerrian::get_height(double x, double y) {
  return perlin_noise_->Noise(x, y, 0);
}