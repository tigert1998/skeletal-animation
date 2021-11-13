//
//  wall.cpp
//  skeletal-animation
//
//  Created by tigertang on 2018/8/5.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "wall.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "utils.h"

using std::string;
using std::vector;
using namespace glm;

Wall::Wall() {
  string vs_source = R"(
#version 410 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
out vec2 vTexCoord;
out vec3 vNormal;
void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
    vTexCoord = aTexCoord;
    vNormal = vec3(uModelMatrix * vec4(aNormal, 0));
}
)";

  string fs_source = R"(
#version 410 core
out vec4 fragColor;
in vec2 vTexCoord;
in vec3 vNormal;
void main() {
    fragColor = vec4(vTexCoord, 1, 1);
}
)";

  shader_ptr_ = std::make_shared<Shader>(vs_source, fs_source);

  vector<VertexType> vertices = {
      {{0, 0, 0}, {0, 0}, {0, 1, 0}},
      {{0, 0, 1}, {0, 1}, {0, 1, 0}},
      {{1, 0, 0}, {1, 0}, {0, 1, 0}},
      {{1, 0, 1}, {1, 1}, {0, 1, 0}},
  };

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType),
                        (void *)offsetof(VertexType, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType),
                        (void *)offsetof(VertexType, tex_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(VertexType),
                        (void *)offsetof(VertexType, normal));

  glBindVertexArray(0);
}

Wall::~Wall() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &vao_);
}

void Wall::Draw(Camera *camera, glm::mat4 model_matrix) const {
  glBindVertexArray(vao_);
  shader_ptr_->Use();
  shader_ptr_->SetUniform("uModelMatrix", model_matrix);
  shader_ptr_->SetUniform("uViewMatrix", camera->view_matrix());
  shader_ptr_->SetUniform("uProjectionMatrix", camera->projection_matrix());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
