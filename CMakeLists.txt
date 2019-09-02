cmake_minimum_required(VERSION 2.8.12)

project(libtoolscurl)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14
add_compile_options(-std=c++11)

#TODO: Add the rest of flags...

set(SOURCE "")
add_subdirectory("${PROJECT_SOURCE_DIR}/src")

add_library(toolscurl STATIC ${SOURCE})

add_executable(main tests/main.cpp)
target_link_libraries(main toolscurl curl)
