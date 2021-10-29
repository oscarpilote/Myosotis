include_directories(
	${Myosotis_SOURCE_DIR}/include/
	)


add_executable(Myosotis 
	main.cpp
	viewer3d.cpp
	trackball.cpp
	)

target_link_libraries(Myosotis OpenGL glfw)

