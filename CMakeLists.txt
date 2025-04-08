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


add_executable (${TARGET})

#set_property(TARGET ${TARGET} PROPERTY C_STANDARD 90)

add_executable (sim-11-test)

# Create trace reader
add_executable (tracereader)

add_subdirectory (src/ms11p)
add_subdirectory (src/cmdlineoptions)
add_subdirectory (src/devicecommon)
add_subdirectory (src/fio)
add_subdirectory (src/unit)
add_subdirectory (src/float)
add_subdirectory (src/bdv11)
add_subdirectory (src/m9312)
add_subdirectory (src/kt24)
add_subdirectory (src/chrono)
add_subdirectory (src/ba11_l)
add_subdirectory (src/ba11_n)
add_subdirectory (src/kd)
add_subdirectory (src/rlv12)
add_subdirectory (src/bus)
add_subdirectory (src/configdata)
add_subdirectory (src/sdl)
add_subdirectory (src/trace)
add_subdirectory (tracereader)
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
add_subdirectory (src/pdp11peripheral)
add_subdirectory (src/interruptrequest)
add_subdirectory (src/signal)
  
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
# the x64-Release configuration (in the folder x64-Release)!
#
# Define source files
set (GTEST_SRCS
    gtest/ashtest.cpp
    gtest/asynctimertest.cpp
    gtest/bitfieldtest.cpp
    gtest/cabinettest.cpp
    gtest/cmdlineoptionstest.cpp
    gtest/threadsafeprioqueuetest.cpp
    gtest/threadsafecontainertest.cpp
    gtest/interruptrequesttest.cpp
    gtest/bitmasktest.cpp
    gtest/busaddresstest.cpp
    gtest/characterbuffertest.cpp
    gtest/conddatatest.cpp
    gtest/conditioncodestest.cpp
    gtest/configprocessortest.cpp
    gtest/consoleconsistencytest.cpp
    gtest/cabinetconsistencytest.cpp
    gtest/ba11_nconfiguratortest.cpp
    gtest/ba11lconfiguratortest.cpp
    gtest/ba11consistencytest.cpp
    gtest/unibusconsistencytest.cpp
    gtest/bdv11configuratortest.cpp
    gtest/dlv11configuratortest.cpp
    gtest/fiotest.cpp
    gtest/fistest.cpp
    gtest/kd11_naconfiguratortest.cpp
    gtest/instructiondecodertest.cpp
    gtest/kd11_na/inctest.cpp
    gtest/kd11_na/movtest.cpp
    gtest/kd11_na/kd11_naregisterstest.cpp
    gtest/kd11_na/kd11_na_odttest.cpp
    gtest/kd11_na/kd11_na_instr_timingtest.cpp
    gtest/kdf11_a/pswtest.cpp
    gtest/kdf11_a/kdf11_aconfiguratortest.cpp
    gtest/kdf11_a/kdf11_a_pswregistertest.cpp
    gtest/kdf11_a/kdf11_aregisterstest.cpp
    gtest/kdf11_a/kdf11_abusdevicetest.cpp
    gtest/kdf11_a/kdf11_a_odttest.cpp
    gtest/kdf11_a/kdf11_instr_timingtest.cpp
    gtest/kdf11_b/kdf11_bconfiguratortest.cpp
    gtest/kdf11_b/sluconfiguratortest.cpp
    gtest/kdf11_b/kdf11b_bdv11configuratortest.cpp
    gtest/kdf11_u/kdf11_uconfiguratortest.cpp
    gtest/kdf11_u/kdf11_udisplayregistertest.cpp
    gtest/ktf11_a/ktf11_atest.cpp
    gtest/locationtest.cpp
    gtest/kt24/kt24configuratortest.cpp
    gtest/kt24/kt24test.cpp
    gtest/m9312/m9312configuratortest.cpp
    gtest/m9312/m9312consistencytest.cpp
    gtest/m9312/m9312test.cpp
    gtest/m9312/m9312powerfailvectortest.cpp
    gtest/memorysizetest.cpp
    gtest/msv11configuratortest.cpp
    gtest/ms11p/ms11pconfiguratortest.cpp
    gtest/ms11p/ms11ptest.cpp
    gtest/operandlocationtest.cpp
    gtest/qbusinstalltest.cpp
    gtest/dummycpu/dummymmu/dummymmu.cpp
    gtest/registers/twowordregistertest.cpp
    gtest/rlv12/rl11configprocessortest.cpp
    gtest/rlv12/rlv11configprocessortest.cpp
    gtest/rlv12/rlv12configprocessortest.cpp
    gtest/rlv12/rlv12attachtest.cpp
    gtest/rlv12/rlv12maintenancetest.cpp
    gtest/rlv12/rlv12getstatustest.cpp
    gtest/rlv12/rlv12readheadertest.cpp
    gtest/rlv12/rlv12readdatatest.cpp
    gtest/rlv12/rlv12readdatanochecktest.cpp
    gtest/rlv12/rlv12seektest.cpp
    gtest/rlv12/rlv12writechecktest.cpp
    gtest/rlv12/rlv12writedatatest.cpp
    gtest/rlv12/volumechecktest.cpp
    gtest/rlv12/drivereadytest.cpp
    gtest/rxconfiguratortest.cpp
    gtest/signaltest.cpp
    gtest/simulatorclocktest.cpp
    gtest/systemconfigtest.cpp
    gtest/tracetest.cpp
    gtest/unibustest.cpp
    gtest/serial/uartmaintmodetest.cpp
    gtest/variantfsmtest.cpp)
    
set (SIM11_TEST_SRCS
    gtest/main/main.cpp
    gtest/cmdlineparser/cmdlineparser.cpp
    ${GTEST_SRCS})

# add_executable (sim-11-test ${SIM11_TEST_SRCS})
target_sources (sim-11-test PRIVATE ${SIM11_TEST_SRCS})

set(GTEST_INCLUDE_DIRS 
    # D:/src/gtest-1.10.0/googletest/include/
    C:/Tools/gtest-1.10.0/include
    )

set (GTESTUNIT_LIBS
    # "D:/src/gtest-1.10.0/build/x64-Release/lib/gtest.lib"
    C:/Tools/gtest-1.10.0/lib/gtest.lib)

set (SDL_INCLUDE_DIRS
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include)

target_include_directories (sim-11-test PUBLIC
    include 
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIRS})
    
target_link_libraries(sim-11-test ${GTESTUNIT_LIBS})

enable_testing ()

target_include_directories (tracereader PRIVATE include src)

# GCC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_compile_options (tracereader PRIVATE -Wall --std=c++20)
endif ()

# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_options (tracereader PRIVATE /std:c++20)
endif ()
    
# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_options (sim-11-test PRIVATE /std:c++20)
endif ()
