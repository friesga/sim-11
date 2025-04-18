#
# N.B. This configuration on x64 only produces a correct executable using
# the x64-Release configuration (in the folder x64-Release)!
#
# Define source files
set (GTEST_SRCS
    ashtest.cpp
    asynctimertest.cpp
    bitfieldtest.cpp
    cabinettest.cpp
    cmdlineoptionstest.cpp
    threadsafeprioqueuetest.cpp
    threadsafecontainertest.cpp
    interruptrequesttest.cpp
    bitmasktest.cpp
    busaddresstest.cpp
    characterbuffertest.cpp
    conddatatest.cpp
    conditioncodestest.cpp
    configprocessortest.cpp
    consoleconsistencytest.cpp
    cabinetconsistencytest.cpp
    ba11_nconfiguratortest.cpp
    ba11lconfiguratortest.cpp
    ba11consistencytest.cpp
    unibusconsistencytest.cpp
    bdv11configuratortest.cpp
    dlv11configuratortest.cpp
    fiotest.cpp
    fistest.cpp
    kd11_naconfiguratortest.cpp
    instructiondecodertest.cpp
    kd11_na/inctest.cpp
    kd11_na/movtest.cpp
    kd11_na/kd11_naregisterstest.cpp
    kd11_na/kd11_na_odttest.cpp
    kd11_na/kd11_na_instr_timingtest.cpp
    kdf11_a/pswtest.cpp
    kdf11_a/kdf11_aconfiguratortest.cpp
    kdf11_a/kdf11_a_pswregistertest.cpp
    kdf11_a/kdf11_aregisterstest.cpp
    kdf11_a/kdf11_abusdevicetest.cpp
    kdf11_a/kdf11_a_odttest.cpp
    kdf11_a/kdf11_instr_timingtest.cpp
    kdf11_b/kdf11_bconfiguratortest.cpp
    kdf11_b/sluconfiguratortest.cpp
    kdf11_b/kdf11b_bdv11configuratortest.cpp
    kdf11_u/kdf11_uconfiguratortest.cpp
    kdf11_u/kdf11_udisplayregistertest.cpp
    ktf11_a/ktf11_atest.cpp
    locationtest.cpp
    kt24/kt24configuratortest.cpp
    kt24/kt24test.cpp
    m9312/m9312configuratortest.cpp
    m9312/m9312consistencytest.cpp
    m9312/m9312test.cpp
    m9312/m9312powerfailvectortest.cpp
    memorysizetest.cpp
    msv11configuratortest.cpp
    ms11p/ms11pconfiguratortest.cpp
    ms11p/ms11ptest.cpp
    operandlocationtest.cpp
    qbusinstalltest.cpp
    dummycpu/dummymmu/dummymmu.cpp
    registers/twowordregistertest.cpp
    rlv12/rl11configprocessortest.cpp
    rlv12/rlv11configprocessortest.cpp
    rlv12/rlv12configprocessortest.cpp
    rlv12/rlv12attachtest.cpp
    rlv12/rlv12maintenancetest.cpp
    rlv12/rlv12getstatustest.cpp
    rlv12/rlv12readheadertest.cpp
    rlv12/rlv12readdatatest.cpp
    rlv12/rlv12readdatanochecktest.cpp
    rlv12/rlv12seektest.cpp
    rlv12/rlv12writechecktest.cpp
    rlv12/rlv12writedatatest.cpp
    rlv12/volumechecktest.cpp
    rlv12/drivereadytest.cpp
    rxconfiguratortest.cpp
    signaltest.cpp
    simulatorclocktest.cpp
    systemconfigtest.cpp
    tracetest.cpp
    unibustest.cpp
    serial/uartmaintmodetest.cpp
    variantfsmtest.cpp)
    
set (SIM11_TEST_SRCS
    main/main.cpp
    cmdlineparser/cmdlineparser.cpp
    ${GTEST_SRCS})

target_sources (sim-11-test PRIVATE ${SIM11_TEST_SRCS})