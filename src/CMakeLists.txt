include_directories(
	${Myosotis_SOURCE_DIR}/include/
	)

add_executable(Myosotis 
	main.cpp
	viewer.cpp
	camera.cpp
	frustum.cpp
	trackball.cpp
	)
target_compile_features(Myosotis PUBLIC cxx_std_17)
target_link_libraries(Myosotis OpenGL glfw)

