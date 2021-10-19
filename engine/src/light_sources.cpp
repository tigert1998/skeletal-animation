#include "light_sources.h"

Directional::Directional(glm::vec3 dir, glm::vec3 color)
    : dir_(dir), color_(color) {}

void Directional::Set(Shader *shader) {
  int id = shader->GetUniform<int32_t>("uDirectionalCount");
  std::string var =
      std::string("uDirectionalLight") + "[" + std::to_string(id) + "]";
  shader->SetUniform<glm::vec3>(var + ".dir", dir_);
  shader->SetUniform<glm::vec3>(var + ".color", color_);
  shader->SetUniform<int32_t>("uDirectionalCount", id + 1);
}

Point::Point(glm::vec3 pos, glm::vec3 color) : pos_(pos), color_(color) {}

void Point::Set(Shader *shader) {
  int id = shader->GetUniform<int32_t>("uPointCount");
  std::string var = std::string("uPointLight") + "[" + std::to_string(id) + "]";
  shader->SetUniform<glm::vec3>(var + ".pos", pos_);
  shader->SetUniform<glm::vec3>(var + ".color", color_);
  shader->SetUniform<int32_t>("uPointCount", id + 1);
}

void LightSources::Add(std::unique_ptr<Light> light) {
  lights_.emplace_back(std::move(light));
}

void LightSources::Set(Shader *shader) {
  for (auto name : std::vector<std::string>{"Directional", "Point"}) {
    shader->SetUniform<int32_t>(std::string("u") + name + "Count", 0);
  }

  for (int i = 0; i < lights_.size(); i++) {
    lights_[i]->Set(shader);
  }
}