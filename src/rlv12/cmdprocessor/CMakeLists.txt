set (CMDPROCESSOR_SRCS
    finishdatatransfercmd.cpp
    cmdprocessor.cpp
    diskaddressok.cpp
    getstatuscmd.cpp
    limitwordcount.cpp
    maintenancecmd.cpp
    readdatacmd.cpp
    readdatawithoutheadercheckcmd.cpp
    readheadercmd.cpp
    run.cpp
    seekcmd.cpp
    writecheckcmd.cpp
    writedatacmd.cpp)

target_sources (${TARGET} PRIVATE ${CMDPROCESSOR_SRCS})
target_sources (sim-11-test PRIVATE ${CMDPROCESSOR_SRCS})
