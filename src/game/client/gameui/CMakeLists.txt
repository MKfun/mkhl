add_sources(
	CMakeLists.txt
	gameui_test_panel.cpp
	gameui_test_panel.h
	gameui_viewport.cpp
	gameui_viewport.h
	imgui_backend.cpp
	imgui_manager.cpp
	imgui_window_system.cpp
	imgui_viewport.cpp
#	imgui_MOTD.cpp
#	imgui_ScorePanel.cpp
	ui_main.cpp
	ui_demo_window.cpp
	ui_crosshairs.cpp
	ui_commands.cpp
	ui_utils.cpp
	${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
	${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
)

add_subdirectory( options )
