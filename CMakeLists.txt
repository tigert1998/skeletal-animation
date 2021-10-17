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

# imgui
set(imgui_dir "third_party/imgui")
include_directories("${imgui_dir}")
set(imgui_srcs 
    "${imgui_dir}/imgui_demo.cpp"
    "${imgui_dir}/imgui_draw.cpp"
    "${imgui_dir}/imgui_tables.cpp"
    "${imgui_dir}/imgui_widgets.cpp"
    "${imgui_dir}/imgui.cpp"
    "${imgui_dir}/backends/imgui_impl_glfw.cpp"
    "${imgui_dir}/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui STATIC "${imgui_srcs}")
target_link_libraries(imgui glfw)

file(GLOB_RECURSE srcs "${src_dir}/*.cpp")
add_executable(main "${srcs}")
target_link_libraries(main glfw glad imgui "${ASSIMP_LIBRARY_DIRS}/${ASSIMP_LIBRARIES}")