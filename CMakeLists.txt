cmake_minimum_required(VERSION 3.10)

# set the project name
project(ministos)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../bin)
# set(CMAKE_CXX_STANDARD 17)

# add the executable
add_executable(ministos src/main.cpp)