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
    src/cmdlineoptions/data.cpp)

set (KD11COMMON_SRCS
src/kd/common/kd11cpudata/kd11cpudata.cpp
    src/kd/common/kd11cpudata/kd11memoryaccess.cpp
    src/kd/common/lsi11instruction/lsi11instruction.cpp)

set (KD11CONTROLLOGIC_SRCS
    src/kd/common/controllogic/controllogic.cpp
    src/kd/common/controllogic/poweruproutine.cpp
    src/kd/common/controllogic/runodt.cpp
    src/kd/common/controllogic/signalreceivers.cpp
    src/kd/common/controllogic/statemachine.cpp)

set (KD11ODT_SRCS
    src/kd/kd11_na/odt/kd11_na_odt.cpp
    src/kd/kd11_na/odt/addressopened.cpp
    src/kd/kd11_na/odt/atprompt.cpp
    src/kd/kd11_na/odt/createevent.cpp
    src/kd/kd11_na/odt/enteringaddress.cpp
    src/kd/kd11_na/odt/enteringaddressvalue.cpp
    src/kd/kd11_na/odt/enteringbinarydump.cpp
    src/kd/kd11_na/odt/enteringregister.cpp
    src/kd/kd11_na/odt/enteringregistervalue.cpp
    src/kd/kd11_na/odt/entrypoint.cpp
    src/kd/kd11_na/odt/exitpoint.cpp
    src/kd/kd11_na/odt/registeropened.cpp
    src/kd/kd11_na/odt/startingregister.cpp)

set (KD11_NA_CPU_SRCS
    src/kd/kd11_na/cpu/kd11_na_cpu.cpp
    src/kd/kd11_na/cpu/cpucontrol.cpp
    src/kd/common/operandlocation/operandlocation.cpp
    src/kd/common/operandlocation/emptyoperandlocation/emptyoperandlocation.cpp
    src/kd/common/operandlocation/registeroperandlocation/registeroperandlocation.cpp
    src/kd/common/operandlocation/memoryoperandlocation/memoryoperandlocation.cpp
    src/kd/common/nooperandinstruction/nooperandinstruction.cpp
    src/kd/common/singleoperandinstruction/singleoperandinstruction.cpp
    src/kd/common/doubleoperandinstruction/doubleoperandinstruction.cpp
    src/kd/common/fisinstruction/fisinstruction.cpp
    src/kd/common/ccinstruction/ccinstruction.cpp
    src/kd/common/branchinstruction/branchinstruction.cpp
    src/kd/common/eisinstruction/eisinstruction.cpp
    src/kd/common/markinstruction/markinstruction.cpp
    src/kd/common/sobinstruction/sobinstruction.cpp
    src/kd/kd11_na/cpu/kd11_nainstruction/kd11_nainstruction.cpp
    src/kd/kd11_na/cpu/kd11_nainstruction/opcodetables.cpp
    src/kd/kd11_na/cpu/kd11_naregisters/kd11_naregisters.cpp)

set (KDF11_A_SRCS
    src/kd/kdf11_a/kdf11_a.cpp
    src/kd/kdf11_a/cpu/kdf11_a_cpu.cpp
    src/kd/kdf11_a/cpu/cpucontrol.cpp
    src/kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.cpp
    src/kd/kdf11_a/cpu/kdf11_ainstruction/opcodetables.cpp
    src/kd/kdf11_a/cpu/kdf11_aregisters/kdf11_aregisters.cpp)

 set (KD11_NA_SRCS
    src/kd/kd11_na/kd11_na.cpp
    src/kd/kd11_na/operatorconsoleaccess/operatorconsoleaccess.cpp
    ${KD11COMMON_SRCS}
    ${KD11CONTROLLOGIC_SRCS}
    ${KD11_NA_CPU_SRCS}
    ${KD11ODT_SRCS})

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
    src/qbus/responsiblemodule.cpp
    src/qbus/signals.cpp)

set (CONFIGURATOR_SRCS
    src/configdata/iniprocessor/iniprocessor.cpp
    src/configdata/iniprocessor/process.cpp
    src/configdata/iniprocessor/processsection.cpp
    src/configdata/iniprocessor/getsystemconfig.cpp
    src/configdata/sectionprocessor/sectionprocessor.cpp
    src/configdata/rlprocessor/rlprocessor.cpp
    src/configdata/deviceconfig/deviceconfig.cpp
    src/configdata/rlconfig/rlconfig.cpp
    src/configdata/rxv21config/rxv21config.cpp
    src/configdata/rxv21processor/rxv21processor.cpp
    src/configdata/rxv21unitconfig/rxv21unitconfig.cpp
    src/configdata/rxv21unitprocessor/rxv21unitprocessor.cpp
    src/configdata/ba11nprocessor/ba11nprocessor.cpp
    src/configdata/ba11nconfig/ba11nconfig.cpp 
    src/configdata/bdv11config/bdv11config.cpp
    src/configdata/bdv11processor/bdv11processor.cpp
    src/configdata/dlv11config/dlv11config.cpp 
    src/configdata/dlv11processor/dlv11processor.cpp
    src/configdata/msv11config/msv11config.cpp 
    src/configdata/msv11processor/msv11processor.cpp
    src/configdata/rlunitconfig/rlunitconfig.cpp
    src/configdata/rlunitprocessor/rlunitprocessor.cpp
    src/configdata/kd11config/kd11config.cpp
    src/configdata/kd11processor/kd11processor.cpp
    src/configdata/kd11_naconfig/kd11_naconfig.cpp
    src/configdata/kd11_naprocessor/kd11_naprocessor.cpp
    src/configdata/kdf11_aconfig/kdf11_aconfig.cpp
    src/configdata/kdf11_aprocessor/kdf11_aprocessor.cpp)

set (FLOAT_SRCS
    src/float/float.cpp
    src/float/ieeetopdp11.cpp
    src/float/pdp11toieee.cpp)

set (BA11_N_SRCS
    src/ba11_n/ba11_n.cpp)

set (SDL_SRCS
    src/sdl/sdlinit/sdlinit.cpp
    src/sdl/sdlpanel/sdlpanel.cpp
    src/sdl/sdlwindow/sdlwindow.cpp
    src/sdl/sdlindicator/sdlindicator.cpp
    src/sdl/sdlfront/sdlfront.cpp
    src/sdl/sdlbutton/sdlbutton.cpp
    src/sdl/sdlrenderer/sdlrenderer.cpp
    src/sdl/sdltexture/sdltexture.cpp
    src/sdl/sdlmomentarybutton/sdlmomentarybutton.cpp
    src/sdl/sdllatchingbutton/sdllatchingbutton.cpp)

set (TRACE_SRCS
    src/trace/trace.cpp
    src/trace/tracefileoutstream/tracefileoutstream.cpp 
    src/trace/tracefileoutstream/header.cpp
    src/trace/tracefileinstream/tracefileinstream.cpp
    src/trace/cpueventrecord/operator.cpp
    src/trace/cpusteprecord/disas.cpp
    src/trace/cpusteprecord/operator.cpp
    src/trace/busrecord/operator.cpp
    src/trace/traprecord/gettrapname.cpp 
    src/trace/traprecord/operator.cpp
    src/trace/irqrecord/operator.cpp 
    src/trace/rxv21diskrecord/operator.cpp 
    src/trace/rxv21commandrecord/getcommandname.cpp
    src/trace/rxv21commandrecord/operator.cpp
    src/trace/rxv21dmarecord/operator.cpp 
    src/trace/rxv21errorrecord/geterrorname.cpp 
    src/trace/rxv21errorrecord/operator.cpp 
    src/trace/dlv11record/operator.cpp 
    src/trace/rlv12registersrecord/tracefileinsertoperator.cpp
    src/trace/rlv12registersrecord/extractionoperator.cpp
    src/trace/rlv12registersrecord/ostreaminsertopreator.cpp 
    src/trace/rlv12commandrecord/operator.cpp
    src/trace/rlv12commandrecord/getcommandname.cpp
    src/trace/durationrecord/tracefileinsertoperator.cpp
    src/trace/durationrecord/extractionoperator.cpp
    src/trace/durationrecord/ostreaminsertoperator.cpp
    src/trace/memorydumprecord/tracefileinsertoperator.cpp 
    src/trace/memorydumprecord/extractionoperator.cpp
    src/trace/memorydumprecord/ostreaminsertoperator.cpp)

# Handling of the console is system-specific
if (MSVC)
set (CONSOLE_SRCS
    src/console/operatorconsole/operatorconsole.cpp
    src/console/operatorconsole/windowsconsole/sender.cpp
    src/console/operatorconsole/windowsconsole/readcharacter.cpp
    src/console/operatorconsole/windowsconsole/getasciichar.cpp)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set (CONSOLE_SRCS
    src/console/operatorconsole/operatorconsole.cpp
    src/console/operatorconsole/linuxconsole/sender.cpp)
endif ()

# Define source files
set (SIM11_SRCS
    ${BA11_N_SRCS}
    ${CONSOLE_SRCS}
    src/abstractbusdevice/writebyte.cpp
    src/bdv11/bdv11.cpp
    src/bdv11/bdv11_e53.cpp
    src/dlv11j/dlv11j.cpp
    ${KD11_NA_SRCS}
    ${KDF11_A_SRCS}
    src/lib/gotapproval.cpp
    src/lsi11/lsi11.cpp
    src/lsi11/configure.cpp
    src/lsi11/checkconsistency.cpp
    src/lsi11/installmodules.cpp
    src/lsi11/loadfile.cpp
    src/lsi11/run.cpp
    src/main/main.cpp
    src/msv11d/msv11d.cpp
    ${QBUS_SRCS}
    src/pdp11peripheral/pdp11peripheral.cpp
    ${RLV12_SRCS}
    ${SDL_SRCS}
    src/rxv21/rxv21.cpp
    src/rxv21/initiatecommand.cpp
    src/rxv21/emptybuffer.cpp
    src/rxv21/fillbuffer.cpp
    src/rxv21/readerrorcode.cpp
    src/rxv21/readsector.cpp
    src/rxv21/readstatus.cpp
    src/rxv21/writesector.cpp
    src/signal/signal.cpp
    ${SIM_FIO_SRCS}
    ${UNIT_SRCS}
    ${TRACE_SRCS}
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
# the x64-Release configuration (in the folder x64-Release)!
#
# Define source files
set (GTEST_SRCS
    gtest/cmdlineoptionstest.cpp
    gtest/threadsafeprioqueuetest.cpp
    gtest/threadsafecontainertest.cpp
    gtest/interruptrequesttest.cpp
    gtest/bitmasktest.cpp
    gtest/conddatatest.cpp
    gtest/conditioncodestest.cpp
    gtest/configuratortest.cpp
    gtest/bdv11configuratortest.cpp
    gtest/dlv11configuratortest.cpp
    gtest/fistest.cpp
    gtest/kd11_naconfiguratortest.cpp
    gtest/kdf11_aconfiguratortest.cpp
    gtest/kd11odttest.cpp
    gtest/kd11_nadecodertest.cpp
    gtest/kd11_na/inctest.cpp
    gtest/kd11_na/movtest.cpp
    gtest/kd11_na/kd11_naregisterstest.cpp
    gtest/kdf11_a/pswtest.cpp
    gtest/kdf11_a/kdf11_aregisterstest.cpp
    gtest/locationtest.cpp
    gtest/operandlocationtest.cpp
    gtest/dummycpu/dummycpu.cpp
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
    gtest/signaltest.cpp
    gtest/tracetest.cpp
    gtest/variantfsmtest.cpp)
    
set (SIM11_TEST_SRCS
    ${GTEST_SRCS}
    ${CMDLINEOPTIONS_SRCS}
    ${CONFIGURATOR_SRCS}
    src/interruptrequest/interruptrequest.cpp
    ${QBUS_SRCS}
    ${UNIT_SRCS}
    ${SIM_FIO_SRCS}
    ${RLV12_SRCS}
    ${FLOAT_SRCS}
    src/abstractbusdevice/writebyte.cpp
    src/pdp11peripheral/pdp11peripheral.cpp
    src/lib/gotapproval.cpp
    ${KD11_NA_SRCS}
    ${KDF11_A_SRCS}
    src/dlv11j/dlv11j.cpp
    src/msv11d/msv11d.cpp
    src/signal/signal.cpp
    ${TRACE_SRCS}
    src/logger/logger.cpp
    src/logger/data.cpp)

add_executable (sim-11-test ${SIM11_TEST_SRCS})

set(GTEST_INCLUDE_DIRS 
    G:/src/gtest-1.10.0/googletest/include)

set (GTESTUNIT_LIBS
    "G:/src/gtest-1.10.0/build/x64-Release/lib/gtest.lib"
    "G:/src/gtest-1.10.0/build/x64-Release/lib/gtest_main.lib")

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

# Create trace reader
add_executable (tracereader 
    tracereader/main.cpp
    ${TRACE_SRCS})

target_include_directories (tracereader PRIVATE include src)
    
# MSVC specific compiler options
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
target_compile_options (sim-11-test PRIVATE /std:c++20)
endif ()
