#
# CMakeList.txt : CMake project for sim-11.
#
cmake_minimum_required (VERSION 3.15)

# Define project
project ("sim-11")

# Define our target once
set (TARGET sim-11)

# Define source files
set (SIM11_SRCS
    src/ba11_n/ba11_n.cpp
    src/bdv11/bdv11.cpp
    src/bdv11/bdv11_e53.cpp
    src/disas.cpp
    src/dlv11j/dlv11j.cpp
    src/kd11/kd11.cpp
    src/lsi11/lsi11.cpp
    src/main.cpp 
    src/msv11d/msv11d.cpp
    src/qbus/qbus.cpp
    src/rxv21/rxv21.cpp
    src/trace.cpp
    src/console.cpp
    src/cmdlineoptions/cmdlineoptions.cpp)

# Add source to this project's executable.
add_executable (${TARGET} ${SIM11_SRCS})
#set_property(TARGET ${TARGET} PROPERTY C_STANDARD 90)

# Add MSVC replacements for linux system calls
if (MSVC)
set (CLOCK_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/contrib/clock_gettime/src/clock_gettime.cpp)
set (TERMIO_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/contrib/termio/src/termio.cpp)
target_sources (${TARGET} PRIVATE ${CLOCK_SRCS} ${TERMIO_SRCS})
endif ()
  
# GCC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_compile_options (${TARGET} PRIVATE 
    -O3 -g -Wall -ffunction-sections -fdata-sections -DUNIX -fsanitize=address)
endif ()

# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_definitions (${TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

# Define directory with include files
target_include_directories (${TARGET} PRIVATE include)

# Windows-specific includes
if (MSVC)
target_include_directories (${TARGET} PRIVATE 
    contrib/termio/include
    contrib/clock_gettime/include
    contrib/sys_time_h
    contrib/unistd_h) 
endif ()

# GCC specific linker options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_link_options (${TARGET} PRIVATE -Wl,-x -Wl,--gc-sections -fsanitize=address)
endif ()