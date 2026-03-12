# used to enforce a minimum version to build this project
cmake_minimum_required(VERSION 4.1.0)

# naming the project + other mumbo jumbo like the version, language(s) used (c++)
project(PetrichorEngine VERSION 0.1.0 LANGUAGES C CXX)

# set function sets a variable to a value; in this case it's the c++ standard that I want to target, c++ 17
# it's also setting the standard to be required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# adding subdirectories which create their own cmakelists.txt files, which build the libraries I want to link to my project
add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL3_image EXCLUDE_FROM_ALL)
# add_subdirectory(external/SDL3_ttf EXCLUDE_FROM_ALL)
add_subdirectory(external/EnTT EXCLUDE_FROM_ALL)
add_subdirectory(external/nlohmann_json EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

# glad is weird because the repo is a generator; i'm adding the generated files manually instead of submoduling
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# getting opengl from the system
find_package(OpenGL REQUIRED) 

# imgui also weird since i can't submodule it; adding manually like glad 
add_library(imgui 
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# where imgui's headers are located, both the main ones and the backends
target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    OpenGL::GL
)

# recursively searching for + collecting files in src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# accessing the list of paths in sources and adding an executable to the build target 
# (meaning: thing i'm building will include an executable) using those paths
add_executable(PetrichorEngine ${SOURCES})

# where to find headers for my code
target_include_directories(PetrichorEngine PRIVATE ${CMAKE_SOURCE_DIR}/include)

# linking found libraries
target_link_libraries(PetrichorEngine PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image
    # SDL3_ttf::SDL3_ttf
    nlohmann_json::nlohmann_json
    glm::glm
    EnTT::EnTT
    glad
    OpenGL::GL
    imgui
)