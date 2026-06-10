add_compile_definitions ( WIN32_LEAN_AND_MEAN )
add_compile_definitions ( HAVE_REMOTE )
add_compile_definitions ( MAKINGDLL )
add_compile_definitions ( QCUSTOMPLOT_USE_LIBRARY )

project(GraphWidget LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Network Concurrent)

add_library(${PROJECT_NAME} STATIC

	src/GraphWidget.h
	src/GraphWidget.cpp

	src/MetricPlotter.h
	src/MetricPlotter.cpp

	src/CustomPlot.h
	src/CustomPlot.cpp

	src/qcustomplot_mod.h
	src/qcustomplot.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../Context/src)

target_link_libraries(${PROJECT_NAME}
	PUBLIC Context
	PUBLIC StarTopologyEmulator
)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Core)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Gui)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::PrintSupport)
target_link_libraries (${PROJECT_NAME} PRIVATE Qt5::Concurrent)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)