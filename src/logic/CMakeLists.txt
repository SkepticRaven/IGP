qt_add_library(
	logic
	STATIC
	ColorCountDialog.cpp
	Pattern.cpp
	Renderer.cpp
	ResizeDialog.cpp
    ColorPicker.cpp
	ColorSlider.cpp

	ColorCountDialog.h
	Pattern.h
	Renderer.h
	ResizeDialog.h
	ColorPicker.h
	ColorSlider.h
)

target_link_libraries(
	logic
	PRIVATE
	Qt6::Core
	Qt6::Widgets
)

target_include_directories(
	logic
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
