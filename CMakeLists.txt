﻿#
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
    src/kd/common/operandlocation/operandlocation.cpp
    src/kd/common/operandlocation/emptyoperandlocation/emptyoperandlocation.cpp
    src/kd/common/operandlocation/registeroperandlocation/registeroperandlocation.cpp
    src/kd/common/operandlocation/memoryoperandlocation/memoryoperandlocation.cpp
    src/kd/common/instructionformats/nooperandinstruction/nooperandinstruction.cpp
    src/kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.cpp
    src/kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.cpp
    src/kd/common/instructionformats/fisinstruction/fisinstruction.cpp
    src/kd/common/instructionformats/ccinstruction/ccinstruction.cpp
    src/kd/common/instructionformats/branchinstruction/branchinstruction.cpp
    src/kd/common/instructionformats/eisinstruction/eisinstruction.cpp
    src/kd/common/instructionformats/markinstruction/markinstruction.cpp
    src/kd/common/instructionformats/sobinstruction/sobinstruction.cpp
    src/kd/common/operanddecoderfactory/operanddecoderfactory.cpp
    src/kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.cpp
    src/kd/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.cpp
    src/kd/common/operanddecoders/branchdecoder/branchdecoder.cpp
    src/kd/common/operanddecoders/doubleoperanddecoder/doubleoperanddecoder.cpp
    src/kd/common/operanddecoders/eisdecoder/eisdecoder.cpp
    src/kd/common/instructiondecoder/instructiondecoder.cpp
    src/kd/common/instructiondecoder/opcodetables.cpp
    src/kd/common/executor/executor.cpp
    src/kd/common/odt/operatorconsoleaccess/operatorconsoleaccess.cpp)

set (DEVICECOMMON_SRCS
    src/devicecommon/registerhandler/registerhandler.cpp)

set (KD11CONTROLLOGIC_SRCS
    src/kd/common/controllogic/controllogic.cpp
    src/kd/common/controllogic/poweruproutine.cpp
    src/kd/common/controllogic/powerdownroutine.cpp
    src/kd/common/controllogic/bootroutine.cpp
    src/kd/common/controllogic/runodt.cpp
    src/kd/common/controllogic/signalreceivers.cpp
    src/kd/common/controllogic/statemachine.cpp)

set (KD11_NA_ODT_SRCS
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
    src/kd/kd11_na/cpucontrol/kd11_na_step.cpp
    src/kd/kd11_na/cpucontrol/kd11_na_cpucontrol.cpp
    src/kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.cpp
    src/kd/kd11_na/pseudommu/pseudommu.cpp
    src/kd/kd11_na/executor/executor.cpp
    src/kd/kd11_na/calculate/calculate.cpp)

set (KDF11_ODT_SRCS
    src/kd/kdf11/odt/kdf11_odt.cpp
    src/kd/kdf11/odt/addressopened.cpp
    src/kd/kdf11/odt/atprompt.cpp
    src/kd/kdf11/odt/createevent.cpp
    src/kd/kdf11/odt/enteringaddress.cpp
    src/kd/kdf11/odt/enteringaddressvalue.cpp
    src/kd/kdf11/odt/enteringbinarydump.cpp
    src/kd/kdf11/odt/enteringregister.cpp
    src/kd/kdf11/odt/enteringregistervalue.cpp
    src/kd/kdf11/odt/entrypoint.cpp
    src/kd/kdf11/odt/exitpoint.cpp
    src/kd/kdf11/odt/registeropened.cpp
    src/kd/kdf11/odt/startingregister.cpp)

set (KDF11_SRCS
    src/kd/kdf11/kdf11_a/kdf11_a.cpp
    src/kd/kdf11/executor/executor.cpp
    src/kd/kdf11/calculate/calculate.cpp
    src/kd/kdf11/kdf11_b/kdf11_b.cpp
    src/kd/kdf11/kdf11_u/kdf11_u.cpp
    src/kd/kdf11/kdf11_u/displayregister/displayregister.cpp
    src/kd/kdf11/serial_line_units/serial_line_units.cpp
    src/kd/kdf11/cpucontrol/kdf11_step.cpp
    src/kd/kdf11/cpucontrol/kdf11_cpucontrol.cpp
    src/kd/kdf11/cpudata/kdf11registers/kdf11registers.cpp
    src/kd/kdf11/cpudata/kdf11busdevice.cpp
    src/kd/kdf11/ktf11_a/ktf11_a.cpp
    src/kd/kdf11/ktf11_a/ktf11_abusdevice.cpp
    ${KDF11_ODT_SRCS})

 set (KD11_NA_SRCS
    src/kd/kd11_na/kd11_na.cpp
    ${KD11COMMON_SRCS}
    ${KD11CONTROLLOGIC_SRCS}
    ${KD11_NA_CPU_SRCS}
    ${KD11_NA_ODT_SRCS})

set (RLV12_SRCS
    src/rlv12/calccrc.cpp
    src/rlv12/rlv12.cpp
    src/rlv12/rl01_02/configure.cpp
    src/rlv12/rl01_02/rl01_02.cpp
    src/rlv12/rl01_02/available.cpp
    src/rlv12/rl01_02/readdata.cpp
    src/rlv12/rl01_02/writedata.cpp
    src/rlv12/rl01_02/readheader.cpp
    src/rlv12/rl01_02/seek.cpp
    src/rlv12/rl01_02/updateheadposition.cpp
    src/rlv12/rl01_02/drivethread.cpp
    src/rlv12/rl01_02/statemachine.cpp
    src/rlv12/rl01_02/createbezel.cpp
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
    src/rlv12/cmdprocessor/writecheckcmd.cpp
    src/rlv12/cmdprocessor/writedatacmd.cpp
    src/rlv12/rlcsplusdrivestatus.cpp
    src/rlv12/read.cpp
    src/rlv12/setdone.cpp
    src/rlv12/writeword.cpp
    src/rlv12/getstatuscmd.cpp)

set (SIM_FIO_SRCS
    src/sim_fio/sim_fopen.cpp
    src/sim_fio/sim_fread.cpp
    src/sim_fio/sim_fsize.cpp
    src/sim_fio/sim_ftell.cpp
    src/sim_fio/sim_fwrite.cpp)

set (UNIT_SRCS     
    src/unit/attach_unit.cpp
    src/unit/createfile.cpp
    src/unit/openreadonly.cpp
    src/unit/openreadwrite.cpp
    src/unit/unit.cpp
    src/unit/createbadblocktable.cpp)

set (BUS_SRCS     
    src/bus/qbus/qbus.cpp
    src/bus/qbus/read.cpp
    src/bus/qbus/writebyte.cpp
    src/bus/qbus/writeword.cpp
    src/bus/qbus/bussignals.cpp
    src/bus/qbus/businterrupts.cpp
    src/bus/qbus/busconfiguration.cpp
    src/bus/unibus/unibus.cpp
    src/bus/unibus/read.cpp
    src/bus/unibus/writeword.cpp
    src/bus/unibus/writebyte.cpp
    src/bus/unibus/bussignals.cpp
    src/bus/unibus/businterrupts.cpp
    src/bus/unibus/busconfiguration.cpp
    src/bus/signalhandler/signalhandler.cpp
    src/bus/configurationhandler/configurationhandler.cpp
    src/bus/configurationhandler/responsiblemodule.cpp
    src/bus/iterator/iterator.cpp
    src/bus/interrupthandler/interrupthandler.cpp)

set (CONFIGURATOR_SRCS
    src/configdata/iniprocessor/process.cpp
    src/configdata/iniprocessor/processsection.cpp
    src/configdata/iniprocessor/getsystemconfig.cpp
    src/configdata/sectionprocessor/sectionprocessor.cpp
    src/configdata/rl/rlprocessor/rlprocessor.cpp
    src/configdata/rl/rl11processor/rl11processor.cpp
    src/configdata/rl/rlv11processor/rlv11processor.cpp
    src/configdata/rl/rlv12processor/rlv12processor.cpp
    src/configdata/rxv21/rxv21processor/rxv21processor.cpp
    src/configdata/rxv21/rxv21unitprocessor/rxv21unitprocessor.cpp
    src/configdata/ba11nconfig/ba11nconfig.cpp
    src/configdata/ba11lconfig/ba11lconfig.cpp
    src/configdata/ba11nprocessor/ba11nprocessor.cpp
    src/configdata/ba11lprocessor/ba11lprocessor.cpp
    src/configdata/bdv11processor/bdv11processor.cpp
    src/configdata/serialconfig/dlv11jconfig/dlv11jconfig.cpp 
    src/configdata/serialconfig/uartconfig/uartconfig.cpp
    src/configdata/serialconfig/dlv11processor/dlv11processor.cpp
    src/configdata/msv11processor/msv11processor.cpp
    src/configdata/rl/rlunitconfig/rlunitconfig.cpp
    src/configdata/rl/rlunitprocessor/rlunitprocessor.cpp
    src/configdata/kd11processor/kd11processor.cpp
    src/configdata/kd11_naprocessor/kd11_naprocessor.cpp
    src/configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.cpp
    src/configdata/kdf11/kdf11_a/kdf11_aprocessor/kdf11_aprocessor.cpp
    src/configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.cpp
    src/configdata/kdf11/kdf11_b/kdf11_bprocessor/kdf11_bprocessor.cpp
    src/configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.cpp
    src/configdata/kdf11/kdf11_u/kdf11_uprocessor/kdf11_uprocessor.cpp
    src/configdata/kdf11/slu/sluprocessor/sluprocessor.cpp
    src/configdata/kt24/kt24processor/kt24processor.cpp
    src/configdata/m9312/m9312processor/m9312processor.cpp
    src/configdata/ms11pprocessor/ms11pprocessor.cpp
    src/configdata/consistencychecker/consistencychecker.cpp
    src/configdata/consistencychecker/checkba11consistency.cpp
    src/configdata/consistencychecker/checkms11consistency.cpp
    src/configdata/consistencychecker/checkkdf11bconsistency.cpp
    src/configdata/consistencychecker/checkcabinetconsistency.cpp
    src/configdata/consistencychecker/checkm9312consistency.cpp
    src/configdata/cabinetprocessor/cabinetprocessor.cpp)

set (FLOAT_SRCS
    src/float/float.cpp
    src/float/ieeetopdp11.cpp
    src/float/pdp11toieee.cpp)

set (CHRONO_SRCS
    src/chrono/simulatorclock/simulatorclock.cpp
    src/chrono/alarmclock/alarmclock.cpp
    src/chrono/wakeupcall/alarmwakeupcall/alarmwakeupcall.cpp)

set (BA11_SRCS
    src/ba11_n/ba11_n.cpp
    src/ba11_l/ba11_l.cpp)

set (BDV11_SRCS
    src/bdv11/bdv11.cpp
    src/bdv11/bdv11_rom.cpp
    src/bdv11/kdf11b_rom.cpp)

set (M9312_SRCS
    src/m9312/m9312.cpp
    src/m9312/romimages.cpp)

set (SDL_SRCS
    src/sdl/sdlinit/sdlinit.cpp
    src/sdl/sdlpanel/sdlpanel.cpp
    src/sdl/sdlwindow/sdlwindow.cpp
    src/sdl/sdlwindow/circle.cpp
    src/sdl/sdlindicator/sdlindicator.cpp
    src/sdl/sdlfront/sdlfront.cpp
    src/sdl/sdlcommonbutton/sdlcommonbutton.cpp
    src/sdl/sdlrenderer/sdlrenderer.cpp
    src/sdl/sdltexture/sdltexture.cpp
    src/sdl/sdlmomentarybutton/sdlmomentarybutton.cpp
    src/sdl/sdllatchingbutton/sdllatchingbutton.cpp
    src/sdl/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp
    src/sdl/sdlthreepositionswitch/sdlthreepositionswitch.cpp
    src/sdl/sdlfourpositionswitch/sdlfourpositionswitch.cpp
    src/sdl/sdlevent/sdlevent.cpp)

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
    src/trace/unibusmaprecord/operator.cpp
    src/trace/clockrecord/tracefileinsertoperator.cpp
    src/trace/clockrecord/extractionoperator.cpp
    src/trace/clockrecord/ostreaminsertoperator.cpp
    src/trace/memorydumprecord/tracefileinsertoperator.cpp 
    src/trace/memorydumprecord/extractionoperator.cpp
    src/trace/memorydumprecord/ostreaminsertoperator.cpp
    src/trace/aprrecord/operator.cpp
    src/trace/mmurecord/operator.cpp
    src/trace/debugrecord/tracefileinsertoperator.cpp
    src/trace/debugrecord/extractionoperator.cpp
    src/trace/debugrecord/ostreaminsertoperator.cpp
    src/trace/ms11precord/operator.cpp)

    set (KT24_SRCS
        src/kt24/kt24.cpp
        src/kt24/unibusmaplogic/unibusmaplogic.cpp)

    set (MS11P_SRCS
        src/ms11p/ms11p.cpp
        src/ms11p/checkbits.cpp
        src/ms11p/errorcorrection.cpp
        src/ms11p/read.cpp
        src/ms11p/write.cpp
        src/ms11p/single_error.cpp
        src/ms11p/double_error.cpp)

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
    ${BA11_SRCS}
    ${CONSOLE_SRCS}
    ${CHRONO_SRCS}
    src/asynctimer/asynctimer.cpp
    src/cabinet/cabinet.cpp
    src/absoluteloader/absoluteloader.cpp
    src/abstractbusdevice/writebyte.cpp
    ${BDV11_SRCS}
    src/dl11/dlv11j/dlv11j.cpp
    src/dl11/uart/uart.cpp
    ${KD11_NA_SRCS}
    ${KDF11_SRCS}
    src/lib/gotapproval.cpp
    src/pdp11/pdp11.cpp
    src/pdp11/configure.cpp
    src/pdp11/installmodules.cpp
    src/pdp11/run.cpp
    src/main/main.cpp
    ${M9312_SRCS}
    src/msv11d/msv11d.cpp
    ${MS11P_SRCS}
    ${BUS_SRCS}
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
    ${KT24_SRCS}
    src/interruptrequest/interruptrequest.cpp
    ${DEVICECOMMON_SRCS})

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
    ${GTEST_SRCS}
    ${CMDLINEOPTIONS_SRCS}
    ${CONFIGURATOR_SRCS}
    ${CHRONO_SRCS}
    src/absoluteloader/absoluteloader.cpp
    src/asynctimer/asynctimer.cpp
    src/interruptrequest/interruptrequest.cpp
    ${BUS_SRCS}
    ${UNIT_SRCS}
    ${SIM_FIO_SRCS}
    ${RLV12_SRCS}
    ${FLOAT_SRCS}
    src/abstractbusdevice/writebyte.cpp
    src/cabinet/cabinet.cpp
    src/pdp11peripheral/pdp11peripheral.cpp
    src/lib/gotapproval.cpp
    ${KD11_NA_SRCS}
    ${KDF11_SRCS}
    ${BDV11_SRCS}
    ${M9312_SRCS}
    src/msv11d/msv11d.cpp
    ${MS11P_SRCS}
    src/dl11/uart/uart.cpp
    ${CONSOLE_SRCS}
    src/signal/signal.cpp
    ${TRACE_SRCS}
    ${KT24_SRCS}
    ${DEVICECOMMON_SRCS}
    src/logger/logger.cpp
    src/logger/data.cpp)

add_executable (sim-11-test ${SIM11_TEST_SRCS})

set(GTEST_INCLUDE_DIRS 
    # D:/src/gtest-1.10.0/googletest/include/
    C:/Tools/gtest-1.10.0/include
    )

set (GTESTUNIT_LIBS
    # "D:/src/gtest-1.10.0/build/x64-Release/lib/gtest.lib"
    # "D:/src/gtest-1.10.0/build/x64-Release/lib/gtest_main.lib"
    C:/Tools/gtest-1.10.0/lib/gtest.lib
    C:/Tools/gtest-1.10.0/lib/gtest_main.lib
    )

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
