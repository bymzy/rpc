
project(RPC)
cmake_minimum_required(VERSION 2.6)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

include_directories(common/include /usr/local/include)
link_directories(${PROJECT_BINARY_DIR}/lib /usr/local/lib)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(common)
add_subdirectory(Test)




