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
	myosotis.cpp
	../extern/miniply/miniply.cpp
	../extern/imgui/imgui.cpp
	../extern/imgui/imgui_draw.cpp
	../extern/imgui/imgui_tables.cpp
	../extern/imgui/imgui_widgets.cpp
	../extern/imgui/imgui_impl_glfw.cpp
	../extern/imgui/imgui_impl_opengl3.cpp
	)

target_compile_features(main PRIVATE cxx_std_17)
target_compile_options(main PRIVATE -Wall -Wextra)
target_link_libraries(main OpenGL glfw ${CMAKE_DL_LIBS})

