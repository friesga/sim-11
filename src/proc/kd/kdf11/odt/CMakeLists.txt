set (KDF11_ODT_SRCS
   kdf11_odt.cpp
   addressopened.cpp
   atprompt.cpp
   createevent.cpp
   enteringaddress.cpp
   enteringaddressvalue.cpp
   enteringbinarydump.cpp
   enteringregister.cpp
   enteringregistervalue.cpp
   entrypoint.cpp
   exitpoint.cpp
   registeropened.cpp
   startingregister.cpp)

target_sources (${TARGET} PRIVATE ${KDF11_ODT_SRCS})
target_sources (sim-11-test PRIVATE ${KDF11_ODT_SRCS})