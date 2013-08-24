cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_NAME "path_tracer")
set(SFML_ROOT "C:\\Program Files\\SFML")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

project(PathTracer2)

file(
	GLOB
	SOURCE_FILES
	src/*.cpp
	src/*.hpp
	src/rgbe/*.cpp
	src/rgbe/*.hpp
)

add_executable(
	${EXECUTABLE_NAME}
	${SOURCE_FILES}
)

add_executable(test_ui src/test_ui/main.cpp)

find_package(SFML 2 COMPONENTS main system window graphics REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
target_link_libraries(test_ui ${SFML_LIBRARIES} glew32 opengl32)
