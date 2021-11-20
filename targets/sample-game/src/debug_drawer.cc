#include "debug_drawer.h"

#include <glad/glad.h>
#include <glog/logging.h>

std::string DebugDrawer::kVsSource = R"(
#version 410 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
out vec3 vColor;
void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";

std::string DebugDrawer::kFsSource = R"(
#version 410 core
in vec3 vColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(vColor, 1.0f);
}
)";

DebugDrawer::DebugDrawer() {
  shader_.reset(new Shader(kVsSource, kFsSource));

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
}

DebugDrawer::~DebugDrawer() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
                           const btVector3 &color) {
  if (!debug_mode_) return;
  glm::vec3 glm_color(color[0], color[1], color[2]);
  vertices_.push_back(Vertex{glm::vec3(from[0], from[1], from[2]), glm_color});
  vertices_.push_back(Vertex{glm::vec3(to[0], to[1], to[2]), glm_color});
}

void DebugDrawer::Draw(Camera *camera) {
  if (!debug_mode_) {
    vertices_.clear();
    return;
  }
  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(),
               vertices_.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));

  shader_->Use();
  shader_->SetUniform<glm::mat4>("uViewMatrix", camera->view_matrix());
  shader_->SetUniform<glm::mat4>("uProjectionMatrix",
                                 camera->projection_matrix());

  glDrawArrays(GL_LINES, 0, vertices_.size());

  vertices_.clear();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void DebugDrawer::reportErrorWarning(const char *warning_string) {
  LOG(WARNING) << "[bullet] " << warning_string;
}

void DebugDrawer::drawContactPoint(const btVector3 &point_on_b,
                                   const btVector3 &normal_on_b,
                                   btScalar distance, int life_time,
                                   const btVector3 &color) {}

void DebugDrawer::draw3dText(const btVector3 &location,
                             const char *text_string) {}
