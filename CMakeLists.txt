cmake_minimum_required(VERSION 3.10)
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM "x64")
endif()
project( swapface )
set(CMAKE_CXX_STANDARD 20)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( swapface 
                video.cpp video.hpp
                SwapFace.cpp SwapFace.hpp
                main.cpp
)
target_link_libraries( swapface ${OpenCV_LIBS} )