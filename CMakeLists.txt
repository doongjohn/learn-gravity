cmake_minimum_required(VERSION 3.19.4)
#                              ^^^^^^ --> minimum version

project("learn-gravity" C)
#       ^^^^^^^^^^^^^^^ ^
#       |               └-> C   uses CMAKE_C_COMPILER
#       |               └-> CXX uses CMAKE_CXX_COMPILER
#       |
#       └-> this will be stored in ${PROJECT_NAME}

# set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

# set flags
# set(CMAKE_C_FLAGS "")
# set(CMAKE_CXX_FLAGS "")

# find all .c files in ./src/gravity
file(GLOB gravityapi "src/gravity/*.c")
add_library(GravityApi "${gravityapi}")
target_link_libraries(GravityApi "-lm")

# make executable
include_directories("src/gravity")
add_executable("${PROJECT_NAME}" "src/main.c")
target_link_libraries("${PROJECT_NAME}" GravityApi "-lm")
