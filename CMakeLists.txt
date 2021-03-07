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

# find all gravity api files
file(GLOB gravityapi
	"$ENV{HOME}/programs/gravity/src/utils/*.c"
	"$ENV{HOME}/programs/gravity/src/shared/*.c"
	"$ENV{HOME}/programs/gravity/src/compiler/*.c"
	"$ENV{HOME}/programs/gravity/src/optionals/*.c"
	"$ENV{HOME}/programs/gravity/src/runtime/*.c"
)
add_library(GravityApi STATIC "${gravityapi}")
target_link_libraries(GravityApi "-lm")

# make executable
include_directories(
	"~/programs/gravity/src/utils"
	"~/programs/gravity/src/shared"
	"~/programs/gravity/src/compiler"
	"~/programs/gravity/src/optionals"
	"~/programs/gravity/src/runtime"
)
add_executable("${PROJECT_NAME}" "src/main.c")
target_link_libraries("${PROJECT_NAME}" GravityApi "-lm")
