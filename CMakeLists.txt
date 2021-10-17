project(skeletal-animation)

set(CMAKE_CXX_FLAGS "-std=c++14")

set(root_dir "skeletal-animation")
set(include_dir "${root_dir}/include")
set(src_dir "${root_dir}/src")
include_directories("${include_dir}")

# glfw
add_subdirectory("third_party/glfw")

# glm
set(glm_dir "third_party/glm")
include_directories("${glm_dir}")

# glad
set(glad_dir "third_party/glad")
include_directories("${glad_dir}/include")
add_library(glad STATIC "${glad_dir}/src/glad.c")

# assimp
find_package(assimp REQUIRED)
include_directories("${ASSIMP_INCLUDE_DIRS}")

# stb
set(stb_dir "third_party/stb")
include_directories("${stb_dir}")

file(GLOB_RECURSE srcs "${src_dir}/*.cpp")
add_executable(main "${srcs}")
target_link_libraries(main glfw glad "${ASSIMP_LIBRARY_DIRS}/${ASSIMP_LIBRARIES}")