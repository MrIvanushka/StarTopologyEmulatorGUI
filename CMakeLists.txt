cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

if(TARGET StarTopologyEmulatorGUI)
	return()
endif()

project(StarTopologyEmulatorGUI LANGUAGES CXX)

find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Gui            REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)

add_subdirectory(StarTopologyEmulator)

add_subdirectory(src/AdvancedDockingSystem)
add_subdirectory(src/Context)
add_subdirectory(src/EmulatorConfigWidget)
add_subdirectory(src/GraphWidget)
add_subdirectory(src/EditorWidget)

#set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")

add_executable(${PROJECT_NAME} main.cpp StarTopologyEmulatorGUI.rc)

target_include_directories(${PROJECT_NAME} PUBLIC src/EditorWidget/src)
target_include_directories(${PROJECT_NAME} PUBLIC src/AdvancedDockingSystem/include)

target_link_libraries(${PROJECT_NAME} PUBLIC EditorWidget)
target_link_libraries(${PROJECT_NAME} PUBLIC Context)
target_link_libraries(${PROJECT_NAME} PUBLIC AdvancedDockingSystem)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Gui)

set_property ( DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})