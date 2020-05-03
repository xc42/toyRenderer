cmake_minimum_required(VERSION 3.12)

project(toyRenderer)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickCore)

add_executable(toyRenderer main.cpp renderer.cpp stl_utils.cpp obj_utils.cpp)
target_include_directories(toyRenderer PUBLIC ${ImageMagick_INCLUDE_DIRS})# ${ImageMagick_Magick++_INCLUDE_DIRS})
target_link_libraries(toyRenderer ${ImageMagick_LIBRARIES})
add_definitions(-DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16)
