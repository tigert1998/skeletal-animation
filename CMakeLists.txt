project(skeletal-animation)

set(CMAKE_CXX_FLAGS "-std=c++14")

add_subdirectory("third_party")

file(GLOB_RECURSE engine_srcs "engine/src/*.cpp" "engine/src/*.cc")
add_library(engine "${engine_srcs}")
target_link_libraries(engine glfw glad imgui glm assimp stb glog::glog bullet)
target_include_directories(engine PUBLIC "engine/include")

add_executable(model-visualizer "model-visualizer/src/main.cc")
target_link_libraries(model-visualizer engine glog::glog)

file(GLOB_RECURSE sample_game_srcs "sample-game/src/*.cc")
add_executable(sample-game "${sample_game_srcs}")
target_include_directories(sample-game PRIVATE "sample-game/include")
target_link_libraries(sample-game engine glog::glog)
