#include "terrain.h"

Terrain::Terrain(const std::string &texture_path) {
  int size = 256;
  double length = 100;

  terrain_.reset(new SimpleSquareTerrain(size, length, texture_path));

  std::vector<glm::vec3> vertices;
  vertices.reserve((size + 1) * (size + 1));
  for (int i = 0; i <= size; i++)
    for (int j = 0; j <= size; j++) {
      double x = 1.0 * i / size * length;
      double z = 1.0 * j / size * length;
      double y = terrain_->get_height(x, z);
      vertices.emplace_back(x, y, z);
    }

  auto bt_vector3_from_glm_vec3 = [](glm::vec3 v) {
    return btVector3(v.x, v.y, v.z);
  };

  bt_triangle_mesh_.reset(new btTriangleMesh());
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++) {
      int a = i * (size + 1) + j;
      int b = i * (size + 1) + j + 1;
      int c = (i + 1) * (size + 1) + j;
      int d = (i + 1) * (size + 1) + j + 1;
      bt_triangle_mesh_->addTriangle(bt_vector3_from_glm_vec3(vertices[a]),
                                     bt_vector3_from_glm_vec3(vertices[b]),
                                     bt_vector3_from_glm_vec3(vertices[c]));
      bt_triangle_mesh_->addTriangle(bt_vector3_from_glm_vec3(vertices[b]),
                                     bt_vector3_from_glm_vec3(vertices[c]),
                                     bt_vector3_from_glm_vec3(vertices[d]));
    }

  shape_ =
      std::make_unique<btBvhTriangleMeshShape>(bt_triangle_mesh_.get(), true);
  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(0, 0, 0));
  motion_state_.reset(new btDefaultMotionState(transform));
  btRigidBody::btRigidBodyConstructionInfo rb_info(
      btScalar(0), motion_state_.get(), shape_.get(), btVector3(0, 0, 0));
  rb_.reset(new btRigidBody(rb_info));
}

void Terrain::Draw(Camera *camera, LightSources *light_sources) {
  terrain_->Draw(camera, light_sources, glm::mat4(1));
}
