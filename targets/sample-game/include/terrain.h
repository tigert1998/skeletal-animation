#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>

#include "terrain/simple_square_terrain.h"

class Terrain {
 private:
  std::unique_ptr<SimpleSquareTerrain> terrain_;

  std::unique_ptr<btTriangleMesh> bt_triangle_mesh_;
  std::unique_ptr<btCollisionShape> shape_;
  std::unique_ptr<btDefaultMotionState> motion_state_;
  std::unique_ptr<btRigidBody> rb_;

 public:
  Terrain(const std::string &texture_path);

  inline btRigidBody *rigid_body() { return rb_.get(); }

  inline SimpleSquareTerrain *terrain() { return terrain_.get(); }

  void Draw(Camera *camera, LightSources *light_sources);
};