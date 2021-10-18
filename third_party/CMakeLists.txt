project(third-party)

# glfw
add_subdirectory("glfw")

# glm
set(glm_dir "glm")
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${glm_dir}")

# glad
set(glad_dir "glad")
add_library(glad STATIC "${glad_dir}/src/glad.c")
target_include_directories(glad PUBLIC "${glad_dir}/include")

# assimp
find_package(assimp REQUIRED)
add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE "${ASSIMP_INCLUDE_DIRS}")
target_link_libraries(assimp INTERFACE "${ASSIMP_LIBRARY_DIRS}/${ASSIMP_LIBRARIES}")

# stb
set(stb_dir "stb")
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${stb_dir}")

# imgui
set(imgui_dir "imgui")
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
target_include_directories(imgui PUBLIC "${imgui_dir}")