set (FIO_SRCS
    fopen.cpp
    fread.cpp
    fseek.cpp
    fsize.cpp
    ftell.cpp
    fwrite.cpp)

target_sources (${TARGET} PRIVATE ${FIO_SRCS})
target_sources (sim-11-test PRIVATE ${FIO_SRCS})