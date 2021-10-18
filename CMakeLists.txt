project(skeletal-animation)

set(CMAKE_CXX_FLAGS "-std=c++14")

add_subdirectory("third_party")

file(GLOB_RECURSE engine_srcs "engine/src/*.cpp")
add_library(engine "${engine_srcs}")
target_link_libraries(engine glfw glad imgui glm assimp stb)
target_include_directories(engine PUBLIC "engine/include")

add_executable(model-visualizer "model-visualizer/src/main.cc")
target_link_libraries(model-visualizer engine)