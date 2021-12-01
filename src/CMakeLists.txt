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
	mesh_grid.cpp
	mesh_utils.cpp
	mesh_remap.cpp
	mesh.cpp
	chrono.cpp
	../extern/miniply/miniply.cpp
	)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main OpenGL glfw)

