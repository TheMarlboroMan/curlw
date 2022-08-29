cmake_minimum_required(VERSION 2.8.12)

project(libtoolscurl)

#None... Building with -Ox causes a segfault 0_o
#set(CMAKE_BUILD_TYPE Release)

#library version
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
add_definitions(-DPATCH_VERSION=${PATCH_VERSION})

message("will build ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")


option(BUILD_SHARED "generates a shared library" ON)
option(BUILD_STATIC "generates a static library" OFF)
option(BUILD_TESTS "builds test programs" OFF)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE "")
include_directories("include")
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

if(${BUILD_STATIC})

	add_library(curlw_static STATIC ${SOURCE})
	set_target_properties(curlw_static PROPERTIES OUTPUT_NAME curlw)
	install(TARGETS curlw_static DESTINATION /usr/local/lib)

	if(${BUILD_TESTS})

		add_executable(main_static tests/main.cpp)
		target_link_libraries(main_static curlw_static curl)
	endif()
endif()

if(${BUILD_SHARED})

	add_library(curlw_shared SHARED ${SOURCE})
	set_target_properties(curlw_shared PROPERTIES OUTPUT_NAME curlw)
	install(TARGETS curlw_shared DESTINATION /usr/local/lib)

	if(${BUILD_TESTS})

		add_executable(main_shared tests/main.cpp)
		target_link_libraries(main_shared curlw_shared curl)
	endif()
endif()

install(DIRECTORY include/ DESTINATION include)
