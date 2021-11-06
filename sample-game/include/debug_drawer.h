#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>
#include <string>
#include <vector>

#include "camera.h"
#include "shader.h"

class DebugDrawer : public btIDebugDraw {
 private:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
  };

  int debug_mode_ = 0;
  std::unique_ptr<Shader> shader_;
  std::vector<Vertex> vertices_;

  uint32_t vao_, vbo_;

  static std::string kVsSource, kFsSource;

 public:
  DebugDrawer();

  ~DebugDrawer();

  void Draw(Camera *camera);

  void drawLine(const btVector3 &from, const btVector3 &to,
                const btVector3 &color) override;

  void reportErrorWarning(const char *warning_string) override;

  void drawContactPoint(const btVector3 &point_on_b,
                        const btVector3 &normal_on_b, btScalar distance,
                        int life_time, const btVector3 &color) override;

  void draw3dText(const btVector3 &location, const char *text_string) override;

  inline void setDebugMode(int debug_mode) override {
    debug_mode_ = debug_mode;
  }

  inline int getDebugMode() const override { return debug_mode_; }
};