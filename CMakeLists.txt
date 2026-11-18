
cmake_minimum_required(VERSION 3.21)

project(3DViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(3DViewer

    src/main.cpp

    src/Camera/Camera.cpp

    src/Model/Mesh.cpp
    src/Model/Model.cpp
    src/Model/ModelLoader.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Shader.cpp
    src/UI/ViewerUI.cpp
)

target_include_directories(3DViewer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(3DViewer PRIVATE
    glfw
    glad::glad
    glm::glm
    assimp::assimp
    nfd::nfd
    imgui::imgui
)

add_custom_command(TARGET 3DViewer POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:3DViewer>/assets/models"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/models/test.obj"
        "$<TARGET_FILE_DIR:3DViewer>/assets/models/test.obj"
    VERBATIM
)

if(MSVC)

    target_compile_options(3DViewer PRIVATE /W4)

else()

    target_compile_options(3DViewer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()
