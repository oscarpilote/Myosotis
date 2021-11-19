include_directories(
	${Myosotis_SOURCE_DIR}/include/
	)

add_executable(main 
	main.cpp
	viewer.cpp
	camera.cpp
	frustum.cpp
	trackball.cpp
	mesh_io.cpp
	../extern/meshoptimizer/src/indexgenerator.cpp
	)

target_compile_features(main PUBLIC cxx_std_20)
target_link_libraries(main OpenGL glfw)

