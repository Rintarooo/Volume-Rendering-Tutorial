cmake_minimum_required(VERSION 3.10)
project(openglcoursejp_project)

find_package(OpenGL REQUIRED)
# find_package(GLFW REQUIRED)
find_package(glfw3 3.2 REQUIRED)
# find_package(glfw3 REQUIRED)
find_package(ImageMagick REQUIRED)
find_package(ImageMagick COMPONENTS convert)
find_package(ImageMagick COMPONENTS convert mogrify display)
find_package(ImageMagick COMPONENTS Magick++)
find_package(ImageMagick COMPONENTS Magick++ convert)

include_directories(
	${OPENGL_INCLUDE_DIR}
	# ${GLFW_INCLUDE_DIR}
	${ImageMagick_INCLUDE_DIRS}
	include/
	Tutorial1/include/
)

add_executable(main 
	Tutorial1/src/main.cpp
	Tutorial1/src/Grid.cpp
	Tutorial1/src/VolumeRenderer.cpp)

target_link_libraries(main 
	${OPENGL_LIBRARIES}
	# ${GLFW_LIBRARIES}
	${ImageMagick_LIBRARIES}
	glfw
)


message("-- OpenGL --")
# https://cmake.org/cmake/help/v3.10/module/FindOpenGL.html
# message(STATUS "version : ${OPENGL_FOUND}")
# message(STATUS "OpenGL version: ${OPENGL_VERSION}")
message(STATUS "libs : ${OPENGL_LIBRARIES}")
message(STATUS "include dirs : ${OPENGL_INCLUDE_DIR}")

