cmake_minimum_required(VERSION 3.15)
project(example)


set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)

add_executable(${PROJECT_NAME} example.c)

target_link_libraries(${PROJECT_NAME} raylib)