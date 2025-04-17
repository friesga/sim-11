set (CONFIGURATOR_SRCS
    iniprocessor/process.cpp
    iniprocessor/processsection.cpp
    iniprocessor/getsystemconfig.cpp
    sectionprocessor/sectionprocessor.cpp
    rl/rlprocessor/rlprocessor.cpp
    rl/rl11processor/rl11processor.cpp
    rl/rlv11processor/rlv11processor.cpp
    rl/rlv12processor/rlv12processor.cpp
    rxv21/rxv21processor/rxv21processor.cpp
    rxv21/rxv21unitprocessor/rxv21unitprocessor.cpp
    ba11nconfig/ba11nconfig.cpp
    ba11lconfig/ba11lconfig.cpp
    ba11nprocessor/ba11nprocessor.cpp
    ba11lprocessor/ba11lprocessor.cpp
    bdv11processor/bdv11processor.cpp
    serialconfig/dlv11jconfig/dlv11jconfig.cpp 
    serialconfig/uartconfig/uartconfig.cpp
    serialconfig/dlv11processor/dlv11processor.cpp
    msv11processor/msv11processor.cpp
    rk/rk11d/rk11dprocessor/rk11dprocessor.cpp
    rk/rk05/rk05config/rk05config.cpp
    rk/rk05/rk05processor/rk05processor.cpp
    rk/rk11d/rk11dprocessor/rk11dprocessor.cpp
    rl/rlunitconfig/rlunitconfig.cpp
    rl/rlunitprocessor/rlunitprocessor.cpp
    kd11processor/kd11processor.cpp
    kd11_naprocessor/kd11_naprocessor.cpp
    kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.cpp
    kdf11/kdf11_a/kdf11_aprocessor/kdf11_aprocessor.cpp
    kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.cpp
    kdf11/kdf11_b/kdf11_bprocessor/kdf11_bprocessor.cpp
    kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.cpp
    kdf11/kdf11_u/kdf11_uprocessor/kdf11_uprocessor.cpp
    kdf11/slu/sluprocessor/sluprocessor.cpp
    kt24/kt24processor/kt24processor.cpp
    m9312/m9312processor/m9312processor.cpp
    ms11pprocessor/ms11pprocessor.cpp
    consistencychecker/consistencychecker.cpp
    consistencychecker/checkba11consistency.cpp
    consistencychecker/checkms11consistency.cpp
    consistencychecker/checkkdf11bconsistency.cpp
    consistencychecker/checkcabinetconsistency.cpp
    consistencychecker/checkm9312consistency.cpp
    cabinetprocessor/cabinetprocessor.cpp)

target_sources (${TARGET} PRIVATE ${CONFIGURATOR_SRCS})
target_sources (sim-11-test PRIVATE ${CONFIGURATOR_SRCS})