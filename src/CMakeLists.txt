include_directories(
	${Myosotis_SOURCE_DIR}/include/
	)

add_executable(Myosotis 
	Main.cpp
	Viewer3d.cpp
	Camera.cpp
	Trackball.cpp
	)

target_link_libraries(Myosotis OpenGL glfw)

