# Handling of the console is system-specific
if (MSVC)
set (CONSOLE_SRCS
    operatorconsole/operatorconsole.cpp
    operatorconsole/windowsconsole/windowsconsole.cpp
    operatorconsole/windowsconsole/sender.cpp
    operatorconsole/windowsconsole/readcharacter.cpp
    operatorconsole/windowsconsole/getasciichar.cpp
    operatorconsole/windowsconsole/print.cpp)
    
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set (CONSOLE_SRCS
    operatorconsole/operatorconsole.cpp
    operatorconsole/linuxconsole/sender.cpp)
endif ()

target_sources (${TARGET} PRIVATE ${CONSOLE_SRCS})
target_sources (sim-11-test PRIVATE ${CONSOLE_SRCS})