#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>

#include "camera.h"
#include "model.h"

class Tank {
 public:
  Tank(const std::string &model_path);

  void Draw(Camera *camera, LightSources *light_sources);

 private:
  std::unique_ptr<Model> model_;

  std::unique_ptr<btCollisionShape> shape_;
  std::unique_ptr<btDefaultMotionState> motion_state_;
  std::unique_ptr<btRigidBody> rb_;
};