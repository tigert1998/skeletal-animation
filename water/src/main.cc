//
//  main.cpp
//  skeletal-animation
//
//  Created by tigertang on 2018/8/1.
//  Copyright © 2018 tigertang. All rights reserved.
//

// clang-format off
#include <glad/glad.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glog/logging.h>
#include <imgui.h>

#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>

#include "debug_quad.h"
#include "keyboard.h"
#include "model.h"
#include "skybox.h"
#include "utils.h"
#include "water.h"

uint32_t width = 1000, height = 600;
float kLength = 8;

using namespace glm;
using namespace std;

std::unique_ptr<Camera> camera_ptr;
std::unique_ptr<LightSources> light_sources_ptr;
std::unique_ptr<Skybox> skybox_ptr;
std::unique_ptr<Water> water;
std::unique_ptr<Model> wall;
std::unique_ptr<DebugQuad> debug_quad;

GLFWwindow *window;

void KeyCallback(GLFWwindow *window, int key, int, int action, int) {
  Keyboard::shared.Trigger(key, action);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  ::width = width / FB_HW_RATIO;
  ::height = height / FB_HW_RATIO;
  camera_ptr->set_width_height_ratio(static_cast<double>(::width) / ::height);
}

void ImGuiInit() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize.x = width;
  io.DisplaySize.y = height;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsClassic();
  auto font = io.Fonts->AddFontDefault();
  io.Fonts->Build();
}

void ImGuiWindow() {
  auto p = camera_ptr->position();
  auto f = camera_ptr->front();
  float p_arr[3] = {p.x, p.y, p.z};
  float f_arr[3] = {f.x, f.y, f.z};
  float alpha = camera_ptr->alpha();
  float beta = camera_ptr->beta();

  ImGui::Begin("Panel");
  ImGui::InputFloat3("camera.position", p_arr);
  ImGui::InputFloat3("camera.front", f_arr);
  ImGui::InputFloat("camera.alpha", &alpha);
  ImGui::InputFloat("camera.beta", &beta);
  ImGui::End();

  camera_ptr->set_position(vec3(p_arr[0], p_arr[1], p_arr[2]));
  camera_ptr->set_front(vec3(f_arr[0], f_arr[1], f_arr[2]));
  camera_ptr->set_alpha(alpha);
  camera_ptr->set_beta(beta);
}

void Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window =
      glfwCreateWindow(width, height, "Water Simulation", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  light_sources_ptr = make_unique<LightSources>();
  light_sources_ptr->Add(
      make_unique<Point>(vec3(100, 100, 100), vec3(1, 1, 1)));

  camera_ptr =
      make_unique<Camera>(vec3(10, 7, 10), static_cast<double>(width) / height);
  camera_ptr->set_front(normalize(vec3(-1, -0.4, -1)));

  wall.reset(new Model("resources/floor/source/floor.obj"));
  water.reset(new Water(100, 100, kLength, 100, 100));
  debug_quad.reset(new DebugQuad());

  skybox_ptr = make_unique<Skybox>("resources/skyboxes/cloud", "png");
  Keyboard::shared.Register([](Keyboard::KeyboardState state, double time) {
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
      camera_ptr->Move(Camera::MoveDirectionType::kRightward,
                       time * move_ratio);
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

  ImGuiInit();
}

void Render(Camera *camera) {
  skybox_ptr->Draw(camera);

  {
    mat4 first = glm::scale(mat4(1), vec3(kLength));
    mat4 move_up = glm::translate(mat4(1), vec3(0, 1, 0));
    mat4 second =
        first * move_up * glm::rotate(glm::pi<float>() / 2, vec3(1, 0, 0));
    mat4 third =
        first * move_up * glm::rotate(-glm::pi<float>() / 2, vec3(0, 0, 1));
    wall->Draw(camera, light_sources_ptr.get(), {first, second, third});
  }
}

int main(int argc, char *argv[]) {
  ::google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

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

    glViewport(0, 0, width * FB_HW_RATIO, height * FB_HW_RATIO);
    Render(camera_ptr.get());

    water->StepSimulation(delta_time);
    water->Draw(camera_ptr.get(), light_sources_ptr.get(), Render,
                glm::translate(mat4(1), vec3(0, kLength * 0.618, 0)));

    glViewport(0, 0, width * FB_HW_RATIO, height * FB_HW_RATIO);
    debug_quad->Draw(water->reflection_tex_id(), vec4(-1, 0, 0, 1));

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGuiWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
  return 0;
}
