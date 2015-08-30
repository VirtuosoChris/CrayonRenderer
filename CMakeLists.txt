cmake_minimum_required(VERSION 2.8)
project(CrayonRenderer)


if(NOT MSVC)
set(CMAKE_CXX_FLAGS "--std=c++0x")
endif(NOT MSVC)

find_package(OpenGL REQUIRED)
add_definitions(-DGLEW_STATIC -DNO_BOOST)
 

find_package(Boost 1.46.0)

#if(Boost_FOUND)
#	include_directories(${Boost_INCLUDE_DIRS})
#else()
#	 message(WARNING "Boost not found")
#	 add_definitions(-DNO_BOOST)	 
#endif()

set(GLEW_INCLUDE_DIR "" CACHE PATH "Glew Include Directory")

set(GL_CPP "" CACHE PATH "GL_CPP")
set(GL_CPP_LIB "GL_CPP_LIB" CACHE FILEPATH "GL_CPP_LIB")

set(VIRTUOSO_MISC_PATH "" CACHE PATH "VIRTUOSO_MISC_PATH")

set(Eigen_DIR "" CACHE PATH "Eigen Include Directory")

set(IMAGELIB_HEADERS "" CACHE PATH "IMAGELIB_HEADERS")
set(IMAGE_LIBRARY_PATH "IMAGELIB" CACHE FILEPATH "IMAGELIB")

set(GLEW_LIBRARY_PATH "GLEW" CACHE FILEPATH "GLEW STATIC LIB")

set(WINDOW_INCLUDE_DIR "" CACHE PATH "Window Include Directory")
set(WINDOW_LIBRARY_PATH "WINDOW_PATH" CACHE FILEPATH "WINDOW STATIC LIB")

set(VIRTUOSO_MODEL_H "" CACHE PATH "VIRTUOSO_MODEL_H")
set(VIRTUOSO_MODEL_LIB "IMAGELIB" CACHE FILEPATH "VIRTUOSO_MODEL_LIB")



include_directories(${GLEW_INCLUDE_DIR})
include_directories(${WINDOW_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GL_CPP})
include_directories(${VIRTUOSO_MISC_PATH})
include_directories(${Eigen_DIR})
include_directories(${IMAGELIB_HEADERS})
include_directories(${VIRTUOSO_MODEL_H})

set (CrayonRendererLib
	 crayonApp.h
	 crayonApp.cpp
	 crayonRenderer.h
	 crayonRenderer.cpp
	 Material.h
	 Material.cpp
	 DefaultMaterials.h
	 DefaultMaterials.cpp

)

add_library(crayon_app ${CrayonRendererLib} )
target_link_libraries(crayon_app ${GL_CPP_LIB})
target_link_libraries(crayon_app ${Boost_LIBRARIES})
target_link_libraries(crayon_app ${OPENGL_gl_LIBRARY})

add_library(crayonPreprocess crayonPreprocess.cpp crayonPreprocess.h)
target_link_libraries(crayonPreprocess ${IMAGE_LIBRARY_PATH})



add_executable(crayonGenerator crayonDataGenerator.cpp)
target_link_libraries(crayonGenerator crayonPreprocess)

add_executable(crayonTest crayonDemo.cpp  ${CrayonRendererLib})
target_link_libraries(crayonTest ${Boost_LIBRARIES})
target_link_libraries(crayonTest ${WINDOW_LIBRARY_PATH})
target_link_libraries(crayonTest ${GLEW_LIBRARY_PATH})
target_link_libraries(crayonTest ${IMAGE_LIBRARY_PATH})
target_link_libraries(crayonTest ${OPENGL_gl_LIBRARY})
target_link_libraries(crayonTest crayon_app)
target_link_libraries(crayonTest ${GL_CPP_LIB})
target_link_libraries(crayonTest ${VIRTUOSO_MODEL_LIB})


add_executable(median median.cpp)
target_link_libraries(median ${WINDOW_LIBRARY_PATH})
target_link_libraries(median ${GLEW_LIBRARY_PATH})
target_link_libraries(median ${OPENGL_gl_LIBRARY})
target_link_libraries(median ${Boost_LIBRARIES})
target_link_libraries(median ${IMAGE_LIBRARY_PATH})
target_link_libraries(median crayon_app)
target_link_libraries(median ${GL_CPP_LIB})
target_link_libraries(median ${VIRTUOSO_MODEL_LIB})




add_executable(swatchGen swatch.cpp)
target_link_libraries(swatchGen ${IMAGE_LIBRARY_PATH})


add_executable(scribbleSquares scribbleSquaresGen.cpp)
target_link_libraries(scribbleSquares ${IMAGE_LIBRARY_PATH})


add_executable(blendPressures blendPressures.cpp)
target_link_libraries(blendPressures ${IMAGE_LIBRARY_PATH})

add_executable(buttonGen buttonGen.cpp)
target_link_libraries(buttonGen ${IMAGE_LIBRARY_PATH})
