cmake_minimum_required(VERSION 2.6)
project(PathTracer2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(SFML_REQUIRED system window graphics)

if (WIN32)
set(SFML_ROOT "C:\\Users\\Alexandre\\Developpement\\libs\\SFML")
list(APPEND SFML_REQUIRED main)
else()
add_definitions(-std=c++11 -msse4.2)
endif()

#========== GLM ==========
include("cmake/glmConfig.cmake")

find_package(SFML 2 COMPONENTS ${SFML_REQUIRED} REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


file(
	GLOB
	PATHTRACER_SOURCE_FILES
	src/pathtracer/*.cpp
	src/pathtracer/*.hpp
	src/pathtracer/rgbe/*.cpp
	src/pathtracer/rgbe/*.hpp
)

include_directories(
	${OpenGL_INCLUDE_DIRS}
	${SFML_INCLUDE_DIR}
	${GLEW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH})

link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

add_executable(path_tracer ${PATHTRACER_SOURCE_FILES})

target_link_libraries(
	path_tracer 
	${SFML_LIBRARIES})

