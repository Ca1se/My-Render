cmake_minimum_required(VERSION 3.0)
project(renderer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.hpp
)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX z xcb xcb-image)
