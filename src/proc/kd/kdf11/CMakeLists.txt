set (KDF11_SRCS
    kdf11_a/kdf11_a.cpp
    executor/executor.cpp
    calculate/calculate.cpp
    kdf11_b/kdf11_b.cpp
    kdf11_u/kdf11_u.cpp
    kdf11_u/displayregister/displayregister.cpp
    serial_line_units/serial_line_units.cpp
    cpucontrol/kdf11_step.cpp
    cpucontrol/kdf11_cpucontrol.cpp
    cpucontrol/kdf11_haltmode.cpp
    cpudata/kdf11registers/kdf11registers.cpp
    cpudata/kdf11busdevice.cpp
    ktf11_a/ktf11_a.cpp
    ktf11_a/ktf11_abusdevice.cpp)

target_sources (${TARGET} PRIVATE ${KDF11_SRCS})
target_sources (sim-11-test PRIVATE ${KDF11_SRCS})

add_subdirectory (odt)
