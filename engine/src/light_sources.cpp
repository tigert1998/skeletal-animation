#include "light_sources.h"

Directional::Directional(glm::vec3 dir, glm::vec3 color)
    : dir_(dir), color_(color) {}

void Directional::Set(Shader *shader) {
  int32_t id = shader->GetUniform<int32_t>("uDirectionalLightCount");
  std::string var =
      std::string("uDirectionalLights") + "[" + std::to_string(id) + "]";
  shader->SetUniform<glm::vec3>(var + ".dir", dir_);
  shader->SetUniform<glm::vec3>(var + ".color", color_);
  shader->SetUniform<int32_t>("uDirectionalLightCount", id + 1);
}

Point::Point(glm::vec3 pos, glm::vec3 color) : pos_(pos), color_(color) {}

void Point::Set(Shader *shader) {
  int32_t id = shader->GetUniform<int32_t>("uPointLightCount");
  std::string var =
      std::string("uPointLights") + "[" + std::to_string(id) + "]";
  shader->SetUniform<glm::vec3>(var + ".pos", pos_);
  shader->SetUniform<glm::vec3>(var + ".color", color_);
  shader->SetUniform<int32_t>("uPointLightCount", id + 1);
}

void LightSources::Add(std::unique_ptr<Light> light) {
  lights_.emplace_back(std::move(light));
}

void LightSources::Set(Shader *shader) {
  for (auto name : std::vector<std::string>{"Directional", "Point"}) {
    shader->SetUniform<int32_t>(std::string("u") + name + "LightCount", 0);
  }

  for (int i = 0; i < lights_.size(); i++) {
    lights_[i]->Set(shader);
  }
}

std::string LightSources::kFsSource = R"(
struct DirectionalLight {
    vec3 dir;
    vec3 color;
};

struct PointLight {
    vec3 pos;
    vec3 color;
};

#define REG_LIGHT(name, count) \
    uniform int u##name##LightCount;      \
    uniform name##Light u##name##Lights[count];

REG_LIGHT(Directional, 1)
REG_LIGHT(Point, 8)

#undef REG_LIGHT
)";