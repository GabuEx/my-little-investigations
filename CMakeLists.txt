
# Minimum Version of CMake Required
cmake_minimum_required(VERSION 2.8)

# Global variables -------------------------------------------------------------
set( global_cxx_flags			"${global_cxx_flags}			-Wall"				)
set( global_cxx_flags_debug		"${global_cxx_flags_debug}		-g -DDEBUG"			)
set( global_cxx_flags_release	"${global_cxx_flags_release}	-s -O2 -DRELEASE"	)

set( root_dir		${CMAKE_CURRENT_SOURCE_DIR}										)
set( src_in_dir		${root_dir}/src													)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY	${bin_out_dir}									)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}									)

set( CMAKE_MODULE_PATH	${CMAKE_CURRENT_SOURCE_DIR}/cmake							)

# My Little Investigations -----------------------------------------------------

# Project properties
project(mli)

# Dependencies
find_package( SDL2			REQUIRED	)
find_package( SDL2_image	REQUIRED	)
find_package( SDL2_ttf		REQUIRED	)
find_package( SDL2_mixer	REQUIRED	)
find_package( FFMPEG 		REQUIRED	)
find_package( Crypto++		REQUIRED	)

# G++ Flags
set( CMAKE_CXX_FLAGS			${global_cxx_flags}			)
set( CMAKE_CXX_FLAGS_DEBUG		${global_cxx_flags_debug}	)
set( CMAKE_CXX_FLAGS_RELEASE	${global_cxx_flags_release}	)

# Sources files list
file( GLOB_RECURSE ${PROJECT_NAME}_src	${src_in_dir}/*.cpp ${src_in_dir}/*.c	)
file( GLOB_RECURSE ${PROJECT_NAME}_head	${src_in_dir}/*.h	)

# Defines
add_definitions( -DGAME_EXECUTABLE )
add_definitions( -DTIXML_USE_TICPP )

if( CMAKE_BUILD_TYPE MATCHES "Debug" )
	add_definitions( -DENABLE_DEBUG_MODE	)
	add_definitions( -DMLI_DEBUG			)
endif()

# Directories for lib openbar-utils link
include_directories	(
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDLMIXER_INCLUDE_DIR}
	#${FFMPEG_INCLUDE_DIR}
	${CRYPTO++_INCLUDE_DIR}
)
link_directories	(
	${FFMPEG_LIBRARY_DIR}
)

# Compile as an executable and link lib openbar-utils
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_src} )
target_link_libraries( ${PROJECT_NAME}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2TTF_LIBRARY}
	${SDLMIXER_LIBRARY}
	${FFMPEG_LIBRARIES}
	${CRYPTO++_LIBRARIES}
)

# ------------------------------------------------------------------------------

