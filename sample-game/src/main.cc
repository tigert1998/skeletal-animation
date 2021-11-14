// clang-format off
#include <glad/glad.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <btBulletDynamicsCommon.h>
#include <glog/logging.h>
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <random>

#include "debug_drawer.h"
#include "keyboard.h"
#include "model.h"
#include "skybox.h"
#include "tank.h"
#include "terrain.h"

uint32_t width = 1000, height = 600;

using namespace glm;
using namespace std;

std::unique_ptr<Model> tree;
std::unique_ptr<Camera> camera_ptr;
std::unique_ptr<LightSources> light_sources_ptr;
std::unique_ptr<Skybox> skybox_ptr;

std::vector<glm::mat4> tree_model_matrices;

std::unique_ptr<Tank> tank;
std::unique_ptr<Terrain> terrain;

std::unique_ptr<btDefaultCollisionConfiguration> collision_config;
std::unique_ptr<btCollisionDispatcher> dispatcher;
std::unique_ptr<btBroadphaseInterface> overlapping_pair_cache;
std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;
std::unique_ptr<DebugDrawer> debug_drawer;

double animation_time = 0;
int animation_id = -1;

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
  static int prev_animation_id = animation_id;
  const char *debug_mode_choices[] = {"off", "on"};
  int debug_mode_choice = debug_drawer->getDebugMode() ? 1 : 0;

  if (prev_animation_id != animation_id) {
    if (0 <= animation_id && animation_id < tree->NumAnimations()) {
      LOG(INFO) << "switching to animation #" << animation_id;
    } else {
      LOG(INFO) << "deactivate animation";
    }
    prev_animation_id = animation_id;
    animation_time = 0;
  }

  ImGui::Begin("Panel");
  ImGui::InputFloat3("camera.position", p_arr);
  ImGui::InputFloat3("camera.front", f_arr);
  ImGui::InputFloat("camera.alpha", &alpha);
  ImGui::InputFloat("camera.beta", &beta);
  ImGui::ListBox("debug mode", &debug_mode_choice, debug_mode_choices,
                 IM_ARRAYSIZE(debug_mode_choices));
  ImGui::End();

  camera_ptr->set_position(vec3(p_arr[0], p_arr[1], p_arr[2]));
  camera_ptr->set_front(vec3(f_arr[0], f_arr[1], f_arr[2]));
  camera_ptr->set_alpha(alpha);
  camera_ptr->set_beta(beta);
  debug_drawer->setDebugMode(debug_mode_choice == 1);
}

void InitWorld() {
  light_sources_ptr = make_unique<LightSources>();
  light_sources_ptr->Add(make_unique<Point>(vec3(5, 100, 5), vec3(1, 1, 1)));

  tree = make_unique<Model>("resources/tree/source/tree.fbx",
                            std::vector<std::string>({"Sphere"}));
  tank = make_unique<Tank>("resources/tiger-i/source/tiger1.obj",
                           glm::vec3(10, 5, 10));
  terrain = make_unique<Terrain>("resources/greenland.png");
  camera_ptr = make_unique<Camera>(vec3(0.5, 0.25, 1),
                                   static_cast<double>(width) / height);
  skybox_ptr = make_unique<Skybox>("resources/skyboxes/cloud", "png");

  std::default_random_engine rd(10086);

  for (int i = 0; i < 10; i++) {
    auto dis = std::uniform_real_distribution<>(0, 100);
    auto x = dis(rd);
    auto z = dis(rd);
    tree_model_matrices.push_back(glm::translate(
        mat4(1), vec3(x, terrain->terrain()->get_height(x, z), z)));
  }

  collision_config.reset(new btDefaultCollisionConfiguration());
  dispatcher.reset(new btCollisionDispatcher(collision_config.get()));
  overlapping_pair_cache.reset(new btDbvtBroadphase());
  solver.reset(new btSequentialImpulseConstraintSolver());
  dynamics_world.reset(new btDiscreteDynamicsWorld(
      dispatcher.get(), overlapping_pair_cache.get(), solver.get(),
      collision_config.get()));
  dynamics_world->setGravity(btVector3(0, -9.8, 0));

  dynamics_world->addRigidBody(terrain->rigid_body());
  dynamics_world->addRigidBody(tank->rigid_body());

  debug_drawer.reset(new DebugDrawer());
  dynamics_world->setDebugDrawer(debug_drawer.get());
}

void Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(width, height, "Sample Game", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

  InitWorld();
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
    animation_time += delta_time;

    glfwPollEvents();
    Keyboard::shared.Elapse(delta_time);
    dynamics_world->stepSimulation(delta_time, 10);
    dynamics_world->debugDrawWorld();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox_ptr->Draw(camera_ptr.get());

    tree->Draw(camera_ptr.get(), light_sources_ptr.get(), tree_model_matrices,
               vec4(0));
    tank->Draw(camera_ptr.get(), light_sources_ptr.get());

    terrain->Draw(camera_ptr.get(), light_sources_ptr.get());
    debug_drawer->Draw(camera_ptr.get());

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
