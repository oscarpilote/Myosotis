cmake_minimum_required(VERSION 3.21)
cmake_policy(VERSION 3.21)

project(Myosotis VERSION 0.1)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
#set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_CXX_STANDARD 17 )

configure_file(
	${PROJECT_SOURCE_DIR}/config/version.h.in
	${PROJECT_BINARY_DIR}/version.h
	)

include_directories(
	${PROJECT_BINARY_DIR}
	)

find_package(OpenGL REQUIRED)
add_compile_definitions(NDC_REVERSED_Z=1)
add_compile_definitions(NDC_REVERSED_Y=0)
add_compile_definitions(NDC_Z_ZERO_ONE=1)
 
find_package(glfw3 REQUIRED)

add_subdirectory(
	src
	)



