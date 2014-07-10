cmake_minimum_required(VERSION 2.8)
project(IFDup)

option(ENABLE_DEBUG "Produce lots of debug information " OFF)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
    )

set(LLVM_RECOMMAND_VERSION "" CACHE STRING "Switch the llvm version")
set_property(CACHE LLVM_RECOMMAND_VERSION PROPERTY STRINGS "" "3.3" "3.4")
find_package(LLVM)

add_subdirectory(lib)

