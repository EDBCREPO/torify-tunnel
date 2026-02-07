cmake_minimum_required(VERSION 3.16)
project( 2D_SVG_RENDERER CXX) 

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")

include(FetchContent)
FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

include(FetchContent)
FetchContent_Declare(
	nodepp-torify
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-torify
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-torify)

add_executable( main ./main.cpp )
target_link_libraries( main PUBLIC nodepp nodepp-torify )
target_include_directories( main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
