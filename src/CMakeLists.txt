# Add source to this project's executable.
set(CMAKE_AUTOUIC ON)
if(WIN32)
	qt_add_executable(IGP WIN32 "main.cpp")
else()
	qt_add_executable(IGP "main.cpp")
endif()

add_subdirectory(app)

add_subdirectory(logic)

target_link_libraries(
	IGP
	PRIVATE
	app
	logic
	Qt6::Core
	Qt6::Widgets
)
