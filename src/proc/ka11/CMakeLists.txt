set (KA11_SRCS
    ka11.cpp
    ka11machinestate/ka11machinestate.cpp
    ky11_a/ky11_a.cpp
    ky11_a/statemachine.cpp
    ky11_a/addressregister/addressregister.cpp
    ky11_a/switchregister/switchregister.cpp
    ky11_a/dataregister/dataregister.cpp
    executor/executor.cpp)

target_sources (${TARGET} PRIVATE ${KA11_SRCS})
target_sources (sim-11-test PRIVATE ${KA11_SRCS})