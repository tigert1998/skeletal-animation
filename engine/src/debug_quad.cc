#include "debug_quad.h"

#include <glad/glad.h>

std::string DebugQuad::kVsSource = R"(
#version 410 core
void main() {}
)";

std::string DebugQuad::kGsSource = R"(
#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 gTexCoord;
uniform vec4 uLocation;

void main() {
    gl_Position = vec4(uLocation.z, uLocation.w, 0.5, 1.0);
    gTexCoord = vec2(1.0, 1.0);
    EmitVertex();

    gl_Position = vec4(uLocation.x, uLocation.w, 0.5, 1.0);
    gTexCoord = vec2(0.0, 1.0); 
    EmitVertex();

    gl_Position = vec4(uLocation.z, uLocation.y, 0.5, 1.0);
    gTexCoord = vec2(1.0, 0.0); 
    EmitVertex();

    gl_Position = vec4(uLocation.x, uLocation.y, 0.5, 1.0);
    gTexCoord = vec2(0.0, 0.0); 
    EmitVertex();

    EndPrimitive(); 
}
)";

std::string DebugQuad::kFsSource = R"(
#version 410 core
uniform sampler2D uTexture;
in vec2 gTexCoord;
out vec4 fragColor;
void main() {
    fragColor = vec4(texture(uTexture, gTexCoord).xyz, 1);
}
)";

std::shared_ptr<Shader> DebugQuad::kShader = nullptr;
uint32_t DebugQuad::vao_ = 0;

DebugQuad::DebugQuad() {
  if (kShader == nullptr) {
    kShader.reset(new Shader({{GL_VERTEX_SHADER, kVsSource},
                              {GL_GEOMETRY_SHADER, kGsSource},
                              {GL_FRAGMENT_SHADER, kFsSource}}));
    glGenVertexArrays(1, &vao_);
  }
  shader_ = kShader;
}

void DebugQuad::Draw(uint32_t texture_id, glm::vec4 location) {
  glBindVertexArray(vao_);
  shader_->Use();
  shader_->SetUniform("uLocation", location);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  shader_->SetUniform<int32_t>("uTexture", 0);
  glDrawArrays(GL_POINTS, 0, 1);
}
