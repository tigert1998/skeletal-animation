//
//  wall.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/5.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "wall.h"

using std::string;
using std::vector;
using namespace glm;

Wall::Wall() {
    string vs_source =
        "#version 410 core\n"
        "layout (location = 0) in vec3 aPosition;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "out vec2 vTexCoord;"
        "void main() {\n"
        "   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);\n"
        "   vTexCoord = aTexCoord;\n"
        "}\n";
    string fs_source =
        "#version 410 core\n"
        "out vec4 fragColor;\n"
        "in vec2 vTexCoord;\n"
        "void main() {\n"
        "   fragColor = vec4(vTexCoord, 0, 1);\n"
        "}\n";
    shader_ptr_ = std::make_shared<Shader>(vs_source, fs_source);
    
    vector<float> vertices = {
        -1, -1, 0, 0, 0,
        -1, 1, 0, 0, 1,
        1, -1, 0, 1, 0,
        1, 1, 0, 1, 1
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Wall::~Wall() {
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &vao_);
}

void Wall::Draw(std::weak_ptr<Camera> camera_ptr) const {
    shader_ptr_->Use();
    shader_ptr_->SetUniform("uModelMatrix", mat4(1));
    shader_ptr_->SetUniform("uViewMatrix", camera_ptr.lock()->view_matrix());
    shader_ptr_->SetUniform("uProjectionMatrix", camera_ptr.lock()->projection_matrix());
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
