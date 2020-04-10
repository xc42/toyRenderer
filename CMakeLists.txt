cmake_minimum_required(VERSION 3.12)

project(toyRenderer)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(toyRenderer renderer.cpp stl_utils.cpp obj_utils)

include_directories("/usr/include/ImageMagick-7" "/usr/include/ImageMagick-7/Magick++")

link_directories("/usr/lib/ImageMagick-7.0.10")

#imagemagick lib
target_link_libraries(toyRenderer MagickCore-7.Q16HDRI Magick++-7.Q16HDRI)
#add_compile_definitions(MAGICKCORE_HDRI_ENABLE 0)
#add_compile_definitions(MAGICKCORE_QUANTUM_DEPTH 16)
