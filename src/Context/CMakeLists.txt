project(Context LANGUAGES CXX)

find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Gui            REQUIRED)

set(Context
	src/Context.h
	src/Context.cpp
)
source_group(Context FILES ${Context})

set(Info
	src/Info.h
	src/Info.cpp
)
source_group(Info FILES ${Info})

set(Export
	src/IContext.h
	src/ModuleWidget.h
)
source_group(Export FILES ${Export})

add_library(${PROJECT_NAME} STATIC
	${Context}
	${Info}
	${Export}
)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Gui)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)