project(AdvancedDockingSystem LANGUAGES CXX)

find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Gui            REQUIRED)

set ( RESOURCES res/ads.qrc)
qt5_add_resources(RESOURCES_QRCS ${RESOURCES})

set(Sources
	src/API.cpp
	src/ContainerWidget.cpp
	src/SectionWidget.cpp
	src/SectionContent.cpp
	src/SectionTitleWidget.cpp
	src/SectionContentWidget.cpp
	src/DropOverlay.cpp
	src/FloatingWidget.cpp
	src/Internal.cpp
	src/Serialization.cpp
)
source_group(Sources FILES ${Sources})

set(Headers
	include/ads/API.h
	include/ads/ContainerWidget.h
	include/ads/SectionWidget.h
	include/ads/SectionContent.h
	include/ads/SectionTitleWidget.h
	include/ads/SectionContentWidget.h
	include/ads/DropOverlay.h
	include/ads/FloatingWidget.h
	include/ads/Internal.h
	include/ads/Serialization.h
)
source_group(Headers FILES ${Headers})

add_library(${PROJECT_NAME} STATIC
	${Sources}
	${Headers}

	${RESOURCES_QRCS}
)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Gui)

target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)