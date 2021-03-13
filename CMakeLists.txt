cmake_minimum_required(VERSION 3.19.4)
#                              ^^^^^^ --> minimum version

project("learn-gravity" C)
#       ^^^^^^^^^^^^^^^ ^
#       |               └-> C   uses CMAKE_C_COMPILER
#       |               └-> CXX uses CMAKE_CXX_COMPILER
#       |
#       └-> this will be stored in ${PROJECT_NAME}

# set output directory
set(
	CMAKE_RUNTIME_OUTPUT_DIRECTORY
	"${CMAKE_SOURCE_DIR}/bin/")

# set flags
# set(CMAKE_C_FLAGS "")
# set(CMAKE_CXX_FLAGS "")

set(gravity_src_dir "$ENV{HOME}/programs/gravity/src")

# find all gravity api files
file(GLOB gravity_api_files
	"${gravity_src_dir}/utils/*.c"
	"${gravity_src_dir}/shared/*.c"
	"${gravity_src_dir}/compiler/*.c"
	"${gravity_src_dir}/optionals/*.c"
	"${gravity_src_dir}/runtime/*.c")
add_library(GravityApi STATIC "${gravity_api_files}")

# make executable
include_directories(
	"${gravity_src_dir}/utils"
	"${gravity_src_dir}/shared"
	"${gravity_src_dir}/compiler"
	"${gravity_src_dir}/optionals"
	"${gravity_src_dir}/runtime")
add_executable("${PROJECT_NAME}" "src/main.c")
target_link_libraries("${PROJECT_NAME}" GravityApi "-lm")
