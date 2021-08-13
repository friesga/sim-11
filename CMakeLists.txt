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
    src/bdv11.c 
    src/bdv11_e53.c 
    src/disas.c 
    src/dlv11j.c 
    src/kd11.c 
    src/lsi11.c 
    src/main.c 
    src/msv11d.c 
    src/rxv21.c 
    src/trace.c)


# Add source to this project's executable.
add_executable (${TARGET} ${SIM11_SRCS})
set_property(TARGET ${TARGET} PROPERTY C_STANDARD 90)

# Define compiler flags
target_compile_options (${TARGET} PRIVATE 
    -O3 -g -Wall -ffunction-sections -fdata-sections -DUNIX -fsanitize=address)

# Define linker flags
target_link_options (${TARGET} PRIVATE 
    -Wl,-x -Wl,--gc-sections -fsanitize=address)

# Define directory with include files
target_include_directories (${TARGET} PRIVATE include) 
