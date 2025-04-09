cmake_minimum_required(VERSION 3.22)
project(NodeApp)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/libs/src)

# vendor/CMakeLists.txt
FetchContent_Declare(
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  # docking-latest
  GIT_TAG docking
  SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/imgui
)

FetchContent_MakeAvailable(imgui)

set(IMGUI_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)


# If using GLAD
set(GLAD_SRC libs/src/glad/glad.c)


# include directories
include_directories(
    libs/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# Build executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${GLAD_SRC}
    ${IMGUI_SRC}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    IMGUI_ENABLE_DOCKING
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

link_directories(
    libs/lib
    libs/src/build/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/libs/lib/libglfw3.a
    opengl32
)

message(STATUS "GLAD Source: ${GLAD_SRC}")
message(STATUS "Sources: ${SOURCES}")