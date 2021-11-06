#include "tank.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

float Tank::kScaling = 0.5;
float Tank::kMass = 54 * 1e3;

Tank::Tank(const std::string &model_path) {
  model_.reset(new Model(model_path));

  shape_.reset(new btBoxShape(btVector3(3.9, 4, 8.9) * kScaling));

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(10, 100, 10));

  btVector3 local_inertia;
  shape_->calculateLocalInertia(kMass, local_inertia);

  motion_state_.reset(new btDefaultMotionState(transform));

  btRigidBody::btRigidBodyConstructionInfo rb_info(kMass, motion_state_.get(),
                                                   shape_.get(), local_inertia);
  rb_.reset(new btRigidBody(rb_info));
}

void Tank::Draw(Camera *camera, LightSources *light_sources) {
  btTransform bt_transform;
  rb_->getMotionState()->getWorldTransform(bt_transform);
  glm::mat4 transform;
  bt_transform.getOpenGLMatrix((btScalar *)&transform);
  auto model_matrix = glm::scale(transform, glm::vec3(kScaling));
  model_->Draw(camera, light_sources, model_matrix);
}

btRigidBody *Tank::rigid_body() { return rb_.get(); }