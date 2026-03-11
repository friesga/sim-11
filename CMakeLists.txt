#
# CMakeList.txt : CMake project for sim-11.
#
cmake_minimum_required (VERSION 3.16)
# cmake_policy(SET CMP0091 NEW)

# Define project
project ("sim-11")
include (GoogleTest)

# Define our build options
option (BUILD_WITH_ASAN "Build with Address Sanitizer" OFF)

# Define our target executables
set (TARGET sim-11)
add_executable (${TARGET})
add_executable (sim-11-test)
add_executable (tracereader)

# Prevent sim-11-test being build on distributed releases
set_target_properties (sim-11-test PROPERTIES EXCLUDE_FROM_ALL true)

# Define the source subdirectories
add_subdirectory (src/ms11p)
add_subdirectory (src/concepts)
add_subdirectory (src/cmdlineoptions)
add_subdirectory (src/devicecommon)
add_subdirectory (src/fio)
add_subdirectory (src/diskdrive)
add_subdirectory (src/float)
add_subdirectory (src/bdv11)
add_subdirectory (src/m792)
add_subdirectory (src/m9312)
add_subdirectory (src/kt24)
add_subdirectory (src/chrono)
add_subdirectory (src/ba11)
add_subdirectory (src/proc)
add_subdirectory (src/rk)
add_subdirectory (src/rlv12)
add_subdirectory (src/bus)
add_subdirectory (src/configdata)
add_subdirectory (src/sdl)
add_subdirectory (src/trace)
add_subdirectory (tracereader_src)
add_subdirectory (src/console)
add_subdirectory (src/rxv21)
add_subdirectory (src/pdp11)
add_subdirectory (src/dl11)
add_subdirectory (src/logger)
add_subdirectory (src/asynctimer)
add_subdirectory (src/cabinet)
add_subdirectory (src/absoluteloader)
add_subdirectory (src/abstractbusdevice)
add_subdirectory (src/main)
add_subdirectory (src/msv11d)
add_subdirectory (src/signal)
add_subdirectory (src/imagedata)
add_subdirectory (gtest)
 

# Definition of include directories and build options for the sim-11 target.
#
# At least C++17 is needed for use of std::variant in variantfsm.
# set(CMAKE_CXX_STANDARD 20) is useless as that not translates to /std:c++20
# but to std:c++latest

target_compile_options (${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: /std:c++20>
    $<$<CXX_COMPILER_ID:GNU>: -pthread -Wall --std=c++20 -ffunction-sections -fdata-sections -DUNIX>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${BUILD_WITH_ASAN}>>: -fsanitize=address>)

target_compile_definitions (${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: CRT_SECURE_NO_WARNINGS>)

target_include_directories (${TARGET} PRIVATE include ${CMAKE_SOURCE_DIR}/src
    $<$<CXX_COMPILER_ID:MSVC>:
        contrib/termio/include
        contrib/clock_gettime/include
        contrib/sys_time_h
        contrib/unistd_h>)

target_link_options (${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>: -pthread -Wl,-x -Wl,--gc-sections>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${BUILD_WITH_ASAN}>>: -fsanitize=address>)

# Build against SDL2 libraries
#
# Note that the target_link_libraries command requires the use of
# semicolon-separated lists.
#
target_include_directories (${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: C:/Libraries/SDL2-2.24.2/include C:/Libraries/SDL2_image-2.6.2/include>
    $<$<CXX_COMPILER_ID:GNU>:  /usr/include/SDL2>)

target_link_libraries (${TARGET}
    "$<$<CXX_COMPILER_ID:MSVC>:C:/Libraries/SDL2-2.24.2/lib/x64/SDL2.lib;C:/Libraries/SDL2_image-2.6.2/lib/x64/SDL2_image.lib>"
    "$<$<CXX_COMPILER_ID:GNU>:SDL2;SDL2_image>")

# The /PROFILE option is needed to be able to collect profiling data.
# Unfortunately profiling doesn't function in Windows 10.
#if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#target_link_options (${TARGET} PRIVATE /PROFILE)
#endif ()

# Definition of include directories, compiler options and libraries for
# the sim-11-test target. This executable is only built for Windows.
#

# Create a statically linked sim-11-test executable
set_property (TARGET sim-11-test
    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set (GTEST_INCLUDE_DIRS 
    # D:/src/gtest-1.10.0/googletest/include/
    C:/Tools/gtest-1.16.0/include)

set (SDL_INCLUDE_DIRS
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include)

target_include_directories (sim-11-test PUBLIC
    include 
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIRS})

target_compile_options (sim-11-test PRIVATE /std:c++20)

set (GTESTUNIT_LIBS
    # "D:/src/gtest-1.10.0/build/x64-Release/lib/gtest.lib"
    C:/Tools/gtest-1.16.0/lib/gtest.lib)
    
target_link_libraries(sim-11-test ${GTESTUNIT_LIBS})

enable_testing ()

target_include_directories (tracereader PRIVATE include src)

# Definition of the compile options for the tracereader target.
target_compile_options (tracereader PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: /std:c++20>
    $<$<CXX_COMPILER_ID:GNU>: -Wall --std=c++20>)

# Build a WebAssembly version of sim-11
add_executable (sdlwindow 
    src/wasm/main.cpp
    src/cabinet/cabinet.cpp
    src/sdl/sdlinit/sdlinit.cpp
    src/sdl/sdlrenderer/sdlrenderer.cpp
    src/sdl/sdlrenderer/circle.cpp
    src/sdl/sdlpanel/sdlpanel.cpp
    src/sdl/sdltexture/sdltexture.cpp
    src/sdl/filepanelbuilder/filepanelbuilder.cpp
    src/sdl/datapanelbuilder/datapanelbuilder.cpp
    src/sdl/sdlfront/sdlfront.cpp
    src/sdl/sdltile/sdltile.cpp
    src/sdl/sdlmultipositionswitch/sdlmultipositionswitch.cpp
    src/sdl/sdlrenderer/sdlrenderer.cpp
    src/sdl/sdlevent/sdlevent.cpp
    src/sdl/sdlwindow/sdlwindow.cpp
    src/sdl/sdlindicator/sdlindicator.cpp
    src/sdl/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp)

target_include_directories (sdlwindow PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    include
    C:/Tools/emsdk-5.0.1/upstream/emscripten/cache/sysroot/include/emscripten
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include)

    # Warnings about unqualified std::move calls are suppressed as Clang
    # detects these incorrectly. See https://reviews.llvm.org/D119670?id=408276
target_compile_options (sdlwindow PRIVATE -std=c++20 -fexceptions -g -Wno-unqualified-std-cast-call)

target_link_options (sdlwindow PRIVATE
    -o sdlwindow.html
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -s ALLOW_MEMORY_GROWTH=1
    -sASSERTIONS=2
    "-sSDL2_IMAGE_FORMATS=[\"png\"]"
    --shell-file ${CMAKE_SOURCE_DIR}/src/sdlwindow.html
    --preload-file ${CMAKE_SOURCE_DIR}/resources/11_03\ front.png@/resources/11_03\ front.png
    -fexceptions
    -g)

# Zorg dat de output file "a.out.js" heet (en dus ook "a.out.wasm" oplevert)
set_target_properties (sdlwindow PROPERTIES
  OUTPUT_NAME "sdlwindow"
  SUFFIX ".html")

# Installation of the sim-11 target
#
# The binary, resources and config directories are all installed in the same
# destination directory. On Windows the SDL2 dll's are installed in that
# directory too.
#
# In Visual Studio 2022 the destination directory is
# CMAKE_SOURCE_DIR\out\install\<Configuration Name>\sim-11; on Linux the
# destination directory is /usr/local/sim-11.
#
install (TARGETS ${TARGET} DESTINATION ./sim-11)
install (DIRECTORY config DESTINATION ./sim-11)
install (DIRECTORY resources DESTINATION ./sim-11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
install (FILES 
    externals/SDL2.dll
    externals/SDL2_image.dll
    externals/SDL2d.dll
    DESTINATION ./sim-11)
endif ()
