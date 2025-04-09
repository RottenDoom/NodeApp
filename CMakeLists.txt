cmake_minimum_required(VERSION 3.22)
project(NodeApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/libs/src)

message(${OpenCV_DIR})

set(OpenCV_DIR set(OpenCV_DIR $ENV{OpenCV_DIR}))
find_package(OpenCV REQUIRED)

# vendor/CMakeLists.txt
FetchContent_Declare(
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  # docking-latest
  GIT_TAG docking
  SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/imgui
)

FetchContent_MakeAvailable(imgui)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
set(GLAD_SRC libs/src/glad/glad.c)

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

# Build executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${GLAD_SRC}
    ${IMGUI_SRC}
)

# include directories
include_directories(${PROJECT_NAME} PRIVATE
    libs/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${OpenCV_INCLUDE_DIRS}
)


target_compile_definitions(${PROJECT_NAME} PRIVATE 
    IMGUI_ENABLE_DOCKING
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/lib
    ${CMAKE_SOURCE_DIR}/libs/src/build/src
)


# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${CMAKE_SOURCE_DIR}/libs/lib/libglfw3.a
    opengl32
)