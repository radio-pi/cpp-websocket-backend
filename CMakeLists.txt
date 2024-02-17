cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2)

project(radiopi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(LIBVLC REQUIRED)
find_package(Crow REQUIRED)

add_subdirectory(src)
