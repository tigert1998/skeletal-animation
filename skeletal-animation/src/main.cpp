//
//  main.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/1.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "sprite_model.h"
#include "wall.h"
#include "keyboard.h"

uint32_t width = 1000, height = 600;

using namespace glm;
using namespace std;

std::shared_ptr<SpriteModel> sprite_model_ptr;
std::shared_ptr<Camera> camera_ptr;

GLFWwindow *window;

void KeyCallback(GLFWwindow *window, int key, int, int action, int) {
    Keyboard::shared.Trigger(key, action);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    ::width = width;
    ::height = height;
    camera_ptr->set_width_height_ratio(static_cast<double>(width) / height);
    glViewport(0, 0, width, height);
}

void Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(width, height, "Skeletal Animation", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sprite_model_ptr = make_shared<SpriteModel>("models/sprite/sprite.fbx");
    camera_ptr = make_shared<Camera>(vec3(0, 16, 56), static_cast<double>(width) / height);
    Keyboard::shared.Register([] (Keyboard::KeyboardState state, double time) {
        double move_ratio = 7;
        if (state[GLFW_KEY_W] || state[GLFW_KEY_UP]) {
            camera_ptr->Move(Camera::MoveDirectionType::kForward, time * move_ratio);
        }
        if (state[GLFW_KEY_S] || state[GLFW_KEY_DOWN]) {
            camera_ptr->Move(Camera::MoveDirectionType::kBackward, time * move_ratio);
        }
        if (state[GLFW_KEY_A] || state[GLFW_KEY_LEFT]) {
            camera_ptr->Move(Camera::MoveDirectionType::kLeftward, time * move_ratio);
        }
        if (state[GLFW_KEY_D] || state[GLFW_KEY_RIGHT]) {
            camera_ptr->Move(Camera::MoveDirectionType::kRightward, time * move_ratio);
        }
        if (state[GLFW_KEY_J]) {
            camera_ptr->Move(Camera::MoveDirectionType::kDownward, time * move_ratio);
        }
        if (state[GLFW_KEY_K]) {
            camera_ptr->Move(Camera::MoveDirectionType::kUpward, time * move_ratio);
        }
        if (state[GLFW_KEY_H]) {
            camera_ptr->Rotate(-time, 0);
        }
        if (state[GLFW_KEY_L]) {
            camera_ptr->Rotate(time, 0);
        }
    });
}

int main() {
    Init();
    while (!glfwWindowShouldClose(window)) {
        static double last_time = glfwGetTime();
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;
        
        glfwPollEvents();
        Keyboard::shared.Elapse(delta_time);
        
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        sprite_model_ptr->Draw(0, camera_ptr, current_time);
        
        glfwSwapBuffers(window);
    }
    return 0;
}
