#
# CMakeList.txt : CMake project for sim-11.
#
cmake_minimum_required (VERSION 3.20)
# cmake_policy(SET CMP0091 NEW)

# Define project
project ("sim-11")
include (GoogleTest)

# Define our target once
set (TARGET sim-11)

# Define source files
set (SIM11_SRCS
    src/ba11_n/ba11_n.cpp
    src/bdv11/bdv11.cpp
    src/bdv11/bdv11_e53.cpp
    src/disas.cpp
    src/dlv11j/dlv11j.cpp
    src/dlv11j/interruptrequest.cpp
    src/kd11/kd11.cpp
    src/kd11/pswpriority.cpp
    src/lib/gotapproval.cpp
    src/lsi11/lsi11.cpp
    src/main.cpp 
    src/msv11d/msv11d.cpp
    src/qbus/qbus.cpp
    src/rlv12/attach.cpp
    src/rlv12/rlv12.cpp
    src/rlv12/rl012.cpp
    src/rlv12/read.cpp
    src/rxv21/rxv21.cpp
    src/rxv21/interruptrequest.cpp
    src/rxv21/initiatecommand.cpp
    src/rxv21/emptybuffer.cpp
    src/rxv21/fillbuffer.cpp
    src/rxv21/readerrorcode.cpp
    src/rxv21/readsector.cpp
    src/rxv21/readstatus.cpp
    src/rxv21/writesector.cpp
    src/sim_fio/sim_fopen.cpp
    src/sim_fio/sim_fread.cpp
    src/sim_fio/sim_fsize.cpp
    src/sim_fio/sim_ftell.cpp
    src/sim_fio/sim_fwrite.cpp
    src/unit/attach_unit.cpp
    src/unit/createfile.cpp
    src/unit/ispipe.cpp
    src/unit/openpipe.cpp
    src/unit/openreadonly.cpp
    src/unit/openreadwrite.cpp
    src/unit/setbuffered.cpp
    src/unit/unit.cpp
    src/unit/createbadblocktable.cpp
    src/trace.cpp
    src/console.cpp
    src/cmdlineoptions/cmdlineoptions.cpp
    src/cmdlineoptions/data.cpp
    src/cmdlineoptions/get.cpp
    src/cmdlineoptions/processoptions.cpp
    src/interruptrequest/interruptrequest.cpp
    src/semaphore/notify.cpp
    src/semaphore/notifyall.cpp
    src/semaphore/semaphore.cpp
    src/semaphore/wait.cpp)

# Add source to this project's executable.
add_executable (${TARGET} ${SIM11_SRCS})
#set_property(TARGET ${TARGET} PROPERTY C_STANDARD 90)

# Add MSVC replacements for linux system calls
if (MSVC)
set (CLOCK_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/contrib/clock_gettime/src/clock_gettime.cpp)
set (TERMIO_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/contrib/termio/src/termio.cpp)
target_sources (${TARGET} PRIVATE ${CLOCK_SRCS} ${TERMIO_SRCS})
endif ()
  
# At least C++17 is needed for use of std::variant in variantfsm.
# set(CMAKE_CXX_STANDARD 20) is useless as that not translates to /std:c++20
# but to std:c++latest

# GCC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_compile_options (${TARGET} PRIVATE 
    -O3 -g -Wall --std=c++20 -ffunction-sections -fdata-sections -DUNIX -fsanitize=address)
endif ()
    
# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_definitions (${TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options (${TARGET} PRIVATE /std:c++20)
endif ()

# Define directory with include files
target_include_directories (${TARGET} PRIVATE include ${CMAKE_SOURCE_DIR}/src)

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

#
# Create sim-11 test executable
#
# N.B. This configuration on x64 only produces a correct executable using
# the x64-Release configuration (in the folder x44-Release)!
#
# Define source files
set (SIM11_TEST_SRCS
    gtest/cmdlineoptionstest.cpp
    src/cmdlineoptions/cmdlineoptions.cpp
    src/cmdlineoptions/data.cpp
    src/cmdlineoptions/get.cpp
    src/cmdlineoptions/processoptions.cpp)

add_executable (sim-11-test ${SIM11_TEST_SRCS})

set(GTEST_INCLUDE_DIRS 
    "G:/src/gtest-1.10.0/googletest/include")

set (GTESTUNIT_LIBS
    "G:/src/gtest-1.10.0/build/x64-Release/lib/gtest.lib"
    "G:/src/gtest-1.10.0/build/x64-Release/lib/gtest_main.lib")

target_include_directories (sim-11-test PUBLIC
    include 
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS})

target_link_libraries(sim-11-test ${GTESTUNIT_LIBS})

enable_testing ()

gtest_add_tests (TARGET sim-11-test 
                 SOURCES ${SIM11_TEST_SRCS}
                 TEST_LIST CmdLineOptionsTest)