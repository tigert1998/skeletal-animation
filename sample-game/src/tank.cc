#include "tank.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Tank::Tank(const std::string &model_path) {
  model_.reset(new Model(model_path));
}

void Tank::Draw(Camera *camera, LightSources *light_sources) {
  auto model_matrix = glm::scale(
      glm::translate(glm::mat4(1), glm::vec3(50, 0, 50)), glm::vec3(0.5));
  model_->Draw(camera, light_sources, model_matrix);
}