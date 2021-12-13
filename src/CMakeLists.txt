include_directories(
	${Myosotis_SOURCE_DIR}/include/
	)

add_executable(myosotis
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
	shaders.cpp
	myosotis.cpp
	../extern/miniply/miniply.cpp
	../extern/imgui/imgui.cpp
	../extern/imgui/imgui_draw.cpp
	../extern/imgui/imgui_tables.cpp
	../extern/imgui/imgui_widgets.cpp
	../extern/imgui/imgui_impl_glfw.cpp
	../extern/imgui/imgui_impl_opengl3.cpp
	)

target_compile_features(myosotis PRIVATE cxx_std_17)
target_compile_options(myosotis PRIVATE -Wall -Wextra)
target_link_libraries(myosotis OpenGL)
target_link_libraries(myosotis glfw)
target_link_libraries(myosotis ${CMAKE_DL_LIBS})
target_link_libraries(myosotis /media/code/Didier/Myosotis/extern/meshoptimizer/build/libmeshoptimizer.a)

