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

# Define our target once
set (TARGET sim-11)

set (CMDLINEOPTIONS_SRCS 
    src/cmdlineoptions/cmdlineoptions.cpp
    src/cmdlineoptions/data.cpp
    src/cmdlineoptions/get.cpp
    src/cmdlineoptions/processoptions.cpp
    src/cmdlineoptions/reset.cpp)

set (RLV12_SRCS
    src/rlv12/calccrc.cpp
    src/rlv12/rlv12.cpp
    src/rlv12/rl01_02/configure.cpp
    src/rlv12/rl01_02/rl012.cpp
    src/rlv12/rl01_02/seektimer.cpp
    src/rlv12/rlv12command/rlv12command.cpp
    src/rlv12/cmdprocessor/finishdatatransfercmd.cpp
    src/rlv12/cmdprocessor/cmdprocessor.cpp
    src/rlv12/cmdprocessor/diskaddressok.cpp
    src/rlv12/cmdprocessor/getstatuscmd.cpp
    src/rlv12/cmdprocessor/limitwordcount.cpp
    src/rlv12/cmdprocessor/maintenancecmd.cpp
    src/rlv12/cmdprocessor/readdatacmd.cpp
    src/rlv12/cmdprocessor/readdatawithoutheadercheckcmd.cpp
    src/rlv12/cmdprocessor/readheadercmd.cpp
    src/rlv12/cmdprocessor/run.cpp
    src/rlv12/cmdprocessor/seekcmd.cpp
    src/rlv12/cmdprocessor/unitavailable.cpp
    src/rlv12/cmdprocessor/updateheadposition.cpp
    src/rlv12/cmdprocessor/writecheckcmd.cpp
    src/rlv12/cmdprocessor/writedatacmd.cpp
    src/rlv12/rlcsplusdrivestatus.cpp
    src/rlv12/read.cpp
    src/rlv12/setdone.cpp
    src/rlv12/writeword.cpp)

set (SIM_FIO_SRCS
    src/sim_fio/sim_fopen.cpp
    src/sim_fio/sim_fread.cpp
    src/sim_fio/sim_fsize.cpp
    src/sim_fio/sim_ftell.cpp
    src/sim_fio/sim_fwrite.cpp)

set (UNIT_SRCS     
    src/unit/attach_unit.cpp
    src/unit/createfile.cpp
    src/unit/ispipe.cpp
    src/unit/openpipe.cpp
    src/unit/openreadonly.cpp
    src/unit/openreadwrite.cpp
    src/unit/setbuffered.cpp
    src/unit/unit.cpp
    src/unit/createbadblocktable.cpp)

set (QBUS_SRCS     
    src/qbus/qbus.cpp
    src/qbus/writebyte.cpp
    src/qbus/writeword.cpp
    src/qbus/responsiblemodule.cpp)

set (CONFIGURATOR_SRCS
    src/configdata/configprocessor/configprocessor.cpp
    src/configdata/rlprocessor/rlprocessor.cpp
    src/configdata/rlprocessor/rlunitprocessor/rlunitprocessor.cpp
    src/configdata/rxprocessor/rxprocessor.cpp
    src/configdata/rxprocessor/rxunitprocessor/rxunitprocessor.cpp
    src/configdata/sectionprocessor/sectionprocessor.cpp)

set (FLOAT_SRCS
    src/float/float.cpp
    src/float/ieeetopdp11.cpp
    src/float/pdp11toieee.cpp)

set (BA11_N_SRCS
    src/ba11_n/ba11_n.cpp
    src/ba11_n/handleevents.cpp
    src/ba11_n/render.cpp)

set (SDLWRAP_SRCS
    src/sdl/renderer/renderer.cpp
    src/sdl/sdlwrap/sdlwrap.cpp
    src/sdl/texture/texture.cpp
    src/sdl/window/window.cpp)

# Handling of the console is system-specific
if (MSVC)
set (CONSOLE_SRCS
    src/console/console.cpp
    src/console/windowsconsole/windowsconsole.cpp
    src/console/windowsconsole/reader.cpp
    src/console/windowsconsole/readcharacter.cpp
    src/console/windowsconsole/isvalidkeyevent.cpp)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set (CONSOLE_SRCS
    src/console/console.cpp
    src/console/linuxconsole/linuxconsole.cpp
    src/console/linuxconsole/reader.cpp)
endif ()

# Define source files
set (SIM11_SRCS
    ${BA11_N_SRCS}
    ${CONSOLE_SRCS}
    src/bdv11/bdv11.cpp
    src/bdv11/bdv11_e53.cpp
    src/disas.cpp
    src/dlv11j/dlv11j.cpp
    src/kd11/kd11.cpp
    src/kd11/cpu/kd11cpu.cpp
    src/kd11/odt/kd11odt.cpp
    src/lib/gotapproval.cpp
    src/lsi11/lsi11.cpp
    src/main.cpp 
    src/msv11d/msv11d.cpp
    ${QBUS_SRCS}
    src/busdevice/writebyte.cpp
    ${RLV12_SRCS}
    ${SDLWRAP_SRCS}
    src/rxv21/rxv21.cpp
    src/rxv21/initiatecommand.cpp
    src/rxv21/emptybuffer.cpp
    src/rxv21/fillbuffer.cpp
    src/rxv21/readerrorcode.cpp
    src/rxv21/readsector.cpp
    src/rxv21/readstatus.cpp
    src/rxv21/writesector.cpp
    ${SIM_FIO_SRCS}
    ${UNIT_SRCS}
    src/trace/trace.cpp
    src/logger/logger.cpp
    src/logger/data.cpp
    ${FLOAT_SRCS}
    ${CMDLINEOPTIONS_SRCS}
    ${CONFIGURATOR_SRCS}
    src/interruptrequest/interruptrequest.cpp)

# Add source to this project's executable.
add_executable (${TARGET} ${SIM11_SRCS})
#set_property(TARGET ${TARGET} PROPERTY C_STANDARD 90)
  
# At least C++17 is needed for use of std::variant in variantfsm.
# set(CMAKE_CXX_STANDARD 20) is useless as that not translates to /std:c++20
# but to std:c++latest

# GCC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_compile_options (${TARGET} PRIVATE 
    -Wall --std=c++20 -ffunction-sections -fdata-sections -DUNIX)

if (BUILD_WITH_ASAN)
target_compile_options (${TARGET} PRIVATE -fsanitize=address)
endif () # BUILD_WITH_ASAN

# Add pthread for gcc
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif () # GCC-specific
    
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
target_link_options (${TARGET} PRIVATE -Wl,-x -Wl,--gc-sections)

if (BUILD_WITH_ASAN)
target_link_options (${TARGET} PRIVATE -fsanitize=address)
endif () # BUILD_WITH_ASAN
endif ()

# Build against SDL2 libraries
if (MSVC)
target_include_directories (${TARGET} PRIVATE 
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include)

target_link_libraries (${TARGET}
    C:/Libraries/SDL2-2.24.2/lib/x64/SDL2.lib
    C:/Libraries/SDL2_image-2.6.2/lib/x64/SDL2_image.lib)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_include_directories (${TARGET} PRIVATE 
    /usr/include/SDL2)

target_link_libraries (${TARGET} SDL2 SDL2_image)
endif ()

# The /PROFILE option is needed to be able to collect profiling data.
# Unfortunately profiling doesn't function in Windows 10.
#if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#target_link_options (${TARGET} PRIVATE /PROFILE)
#endif ()

#
# Create sim-11 test executable
#
# N.B. This configuration on x64 only produces a correct executable using
# the x64-Release configuration (in the folder x44-Release)!
#
# Define source files
set (SIM11_TEST_SRCS
    gtest/cmdlineoptionstest.cpp
    gtest/threadsafeprioqueuetest.cpp
    gtest/threadsafecontainertest.cpp
    gtest/interruptrequesttest.cpp
    gtest/bitmasktest.cpp
    gtest/configuratortest.cpp
    gtest/fistest.cpp
    gtest/rlv12attachtest.cpp
    gtest/rlv12maintenancetest.cpp
    gtest/rlv12getstatustest.cpp
    gtest/rlv12readheadertest.cpp
    gtest/rlv12readdatatest.cpp
    gtest/rlv12readdatanochecktest.cpp
    gtest/rlv12seektest.cpp
    gtest/rlv12writechecktest.cpp
    gtest/rlv12writedatatest.cpp
    gtest/rxconfiguratortest.cpp
    ${CMDLINEOPTIONS_SRCS}
    ${CONFIGURATOR_SRCS}
    src/interruptrequest/interruptrequest.cpp
    ${QBUS_SRCS}
    ${UNIT_SRCS}
    ${SIM_FIO_SRCS}
    ${RLV12_SRCS}
    ${FLOAT_SRCS}
    src/busdevice/writebyte.cpp
    src/lib/gotapproval.cpp
    src/lsi11/lsi11.cpp
    src/kd11/kd11.cpp
    src/kd11/cpu/kd11cpu.cpp
    src/kd11/odt/kd11odt.cpp
    src/msv11d/msv11d.cpp
    src/trace/trace.cpp
    src/logger/logger.cpp
    src/logger/data.cpp
    src/disas.cpp)

add_executable (sim-11-test ${SIM11_TEST_SRCS})

set(GTEST_INCLUDE_DIRS 
    G:/src/gtest-1.10.0/googletest/include)

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
                 TEST_LIST 
                    BitmaskTest
                    CmdLineOptionsTest
                    ThreadSafePrioQueue
                    InterruptRequest
                    ConfigProcessorTest
                    FISTest
                    RLV12MaintenanceTest
                    RLV12AttachTest
                    RLV12WriteTest
                    RLV12GetStatusTest
                    RLV12ReadHeaderTest
                    RLV12SeekTest
                    RLV12ReadDataTest
                    RLV12WriteDataTest
                    RLV12WriteCheckTest
                    RLV12ReadDataWithoutHeaderCheckTest
                    RxConfiguratorTest)

#
# Create trace reader
add_executable (tracereader tracereader/main.cpp
    src/trace/trace.cpp
    src/disas.cpp)

target_include_directories (tracereader PRIVATE include src)
    
# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_options (sim-11-test PRIVATE /std:c++20)
endif ()
