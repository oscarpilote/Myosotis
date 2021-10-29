cmake_minimum_required(VERSION 3.0)
project(Myosotis VERSION 0.1)

configure_file(
	${PROJECT_SOURCE_DIR}/version.h.in
	${PROJECT_BINARY_DIR}/version.h
	)

include_directories(
	${PROJECT_BINARY_DIR}
	)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(
	src
	)



