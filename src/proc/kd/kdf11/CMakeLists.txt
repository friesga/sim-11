set (KDF11_SRCS
    kdf11_a/kdf11_a.cpp
    executor/executor.cpp
    calculate/calculate.cpp
    kdf11_b/kdf11_b.cpp
    kdf11_u/kdf11_u.cpp
    kdf11_u/displayregister/displayregister.cpp
    serial_line_units/serial_line_units.cpp
    kdf11_executionengine/kdf11_executionengine.cpp
    kdf11processorexception/kdf11processorexception.cpp
    haltmode/haltmode.cpp
    cpudata/kdf11registers/kdf11registers.cpp
    cpudata/kdf11busdevice.cpp
    ktf11_a/ktf11_a.cpp
    ktf11_a/ktf11_abusdevice.cpp)

target_sources (${TARGET} PRIVATE ${KDF11_SRCS})
target_sources (sim-11-test PRIVATE ${KDF11_SRCS})

add_subdirectory (odt)
