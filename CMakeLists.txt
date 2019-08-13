cmake_minimum_required (VERSION 2.8)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (HeadShake)

# Options
option(CMP_LIN "CMP_LIN" OFF)
option(CMP_WIN_64 "CMP_WIN_64" OFF)
option(CMP_MAC "CMP_MAC" OFF)

# Set the libraries path: edit this line to specify your custom library path
get_filename_component(SDK_PATH "../SDK/" ABSOLUTE)
message("-- Detecting X-Plane SDK path")
if(NOT EXISTS ${SDK_PATH})
	  message(FATAL_ERROR "Missing SDK folder: ${SDK_PATH}")
endif(NOT EXISTS ${SDK_PATH})

configure_file("src/collisions" "collisions" COPYONLY)

# Set the default compiler settings
set (GCC_DEFAULT_FLAGS -std=c++0x -Wall -Wunreachable-code -pipe -Wextra -Wshadow -Wfloat-equal -pedantic -fvisibility=hidden -O2 -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -W -DXPLM200 -DXPLM210)

# Include files
include_directories ("./src")
include_directories ("${SDK_PATH}")
include_directories ("${SDK_PATH}/CHeaders/XPLM")
include_directories ("${SDK_PATH}/CHeaders/Wrappers")
include_directories ("${SDK_PATH}/CHeaders/Widgets")
file(GLOB_RECURSE SOURCES "src/*.cpp")

if(CMP_LIN)
	# ------------------
	# Release - Linux 64
	# ------------------
	add_library(LIN_RELEASE_64 SHARED ${SOURCES})

	target_compile_options(LIN_RELEASE_64 PRIVATE ${GCC_DEFAULT_FLAGS} -m64 -fPIC -DLIN)
	set_target_properties(LIN_RELEASE_64 PROPERTIES LINK_FLAGS "-Wl,--version-script=collisions")
	set_target_properties(LIN_RELEASE_64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../bin/Release/HeadShake/64/")
	set_target_properties(LIN_RELEASE_64 PROPERTIES PREFIX "")
	set_target_properties(LIN_RELEASE_64 PROPERTIES OUTPUT_NAME "lin.xpl")
	set_target_properties(LIN_RELEASE_64 PROPERTIES SUFFIX "")

	# ------------------
	# Debug - Linux 64
	# ------------------
	add_library(LIN_DEBUG_64 SHARED ${SOURCES})

	target_compile_options(LIN_DEBUG_64 PRIVATE ${GCC_DEFAULT_FLAGS} -m64 -fPIC -g -DLIN)
	set_target_properties(LIN_DEBUG_64 PROPERTIES LINK_FLAGS "-Wl,--version-script=collisions")
	set_target_properties(LIN_DEBUG_64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../bin/Debug/HeadShake/64/")
	set_target_properties(LIN_DEBUG_64 PROPERTIES PREFIX "")
	set_target_properties(LIN_DEBUG_64 PROPERTIES OUTPUT_NAME "lin.xpl")
	set_target_properties(LIN_DEBUG_64 PROPERTIES SUFFIX "")
endif(CMP_LIN)

if(CMP_WIN_64)
	cmake_policy(SET CMP0015 NEW)
	set (CMAKE_SYSTEM_NAME Windows)
	set (CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
	link_directories("${SDK_PATH}/Libraries/Win")

	# --------------------
	# Release - Windows 64
	# --------------------
	add_library(WIN_RELEASE_64 SHARED ${SOURCES})

	find_package(Threads REQUIRED)
	target_compile_options(WIN_RELEASE_64 PRIVATE ${GCC_DEFAULT_FLAGS} -s -c -fno-stack-protector -static-libgcc -static-libstdc++ -DXPLM200 -DIBM)
	target_link_libraries(WIN_RELEASE_64 XPLM_64 XPWidgets_64 -static-libgcc -static-libstdc++ -static ${CMAKE_THREAD_LIBS_INIT})
	set_target_properties(WIN_RELEASE_64 PROPERTIES LINK_FLAGS "-s -Wl,--version-script=collisions")
	set_target_properties(WIN_RELEASE_64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../bin/Release/HeadShake/64/")
	set_target_properties(WIN_RELEASE_64 PROPERTIES PREFIX "")
	set_target_properties(WIN_RELEASE_64 PROPERTIES OUTPUT_NAME "win.xpl")
	set_target_properties(WIN_RELEASE_64 PROPERTIES SUFFIX "")
endif(CMP_WIN_64)

if (CMP_MAC)
	cmake_policy(SET CMP0042 NEW)
	set (CMAKE_SYSTEM_NAME Darwin)
	set (CMAKE_CXX_COMPILER g++)

	# --------------------
	# Release - Mac 64
	# --------------------
	add_library(MAC_RELEASE_64 SHARED ${SOURCES})

	target_compile_options(MAC_RELEASE_64 PRIVATE ${GCC_DEFAULT_FLAGS} -arch x86_64 -nostdinc++ -I/Library/Developer/CommandLineTools/usr/include/c++/v1 -DXPLM200 -DAPL)
	target_link_libraries(MAC_RELEASE_64 "-F${SDK_PATH}/Libraries/Mac" "-framework XPLM" "-framework XPWidgets" "-framework OpenGL" "-nodefaultlibs" "-lc++" "-lc++abi" "-lm" "-lc")
	set_target_properties(MAC_RELEASE_64 PROPERTIES LINK_FLAGS "-m64 -fvisibility=hidden")
	set_target_properties(MAC_RELEASE_64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../bin/Release/HeadShake/64/")
	set_target_properties(MAC_RELEASE_64 PROPERTIES PREFIX "")
	set_target_properties(MAC_RELEASE_64 PROPERTIES OUTPUT_NAME "mac.xpl")
	set_target_properties(MAC_RELEASE_64 PROPERTIES SUFFIX "")
endif (CMP_MAC)
