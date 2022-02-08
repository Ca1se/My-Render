cmake_minimum_required(VERSION 3.0)
project(render)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cc
        ${PROJECT_SOURCE_DIR}/src/*.hh
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} z xcb)