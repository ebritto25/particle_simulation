cmake_minimum_required(VERSION 2.8.12)

project(particle_simulation)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


add_executable(${PROJECT_NAME} "main.cpp" "screen.cpp" "swarm.cpp")

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

