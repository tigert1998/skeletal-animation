project(skeletal-animation)

set(CMAKE_CXX_FLAGS "-std=c++14")

add_subdirectory("third_party")

file(GLOB_RECURSE engine_srcs "engine/src/*.cpp" "engine/src/*.cc")
add_library(engine "${engine_srcs}")
target_link_libraries(engine glfw glad imgui glm assimp stb glog::glog)
target_include_directories(engine PUBLIC "engine/include")

add_executable(model-visualizer "targets/model-visualizer/src/main.cc")
target_link_libraries(model-visualizer engine glog::glog)

file(GLOB_RECURSE sample_game_srcs "targets/sample-game/src/*.cc")
add_executable(sample-game "${sample_game_srcs}")
target_include_directories(sample-game PRIVATE "targets/sample-game/include")
target_link_libraries(sample-game engine glog::glog bullet)

file(GLOB_RECURSE water_srcs "targets/water/src/*.cc")
add_executable(water "${water_srcs}")
target_include_directories(water PRIVATE "targets/water/include")
target_link_libraries(water engine glog::glog)
