cmake_minimum_required(VERSION 2.8.12)

project(libtoolscurl)

#None... Building with -Ox causes a segfault 0_o
#set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE "")
add_subdirectory("${PROJECT_SOURCE_DIR}/src")

add_library(toolscurl_st STATIC ${SOURCE})

add_library(toolscurl_sh SHARED ${SOURCE})
install(TARGETS toolscurl_sh DESTINATION /usr/local/lib)
install(DIRECTORY src/ DESTINATION /usr/local/include/toolscurl FILES_MATCHING PATTERN "*.h")

add_executable(main_static tests/main.cpp)
target_link_libraries(main_static toolscurl_st curl)

add_executable(main_shared tests/main.cpp)
target_link_libraries(main_shared toolscurl_sh curl)
