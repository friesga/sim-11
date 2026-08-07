target_compile_features(${TARGET} PRIVATE cxx_std_20)
target_compile_features(${SIM11_TEST_TARGET} PRIVATE cxx_std_20)
target_compile_features(${SIM11_WASM_TARGET} PRIVATE cxx_std_20)
target_compile_features(${TRACEREADER_TARGET} PRIVATE cxx_std_20)

target_include_directories(${TARGET} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)

target_include_directories(${TRACEREADER_TARGET} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)

add_subdirectory(src/concepts)
add_subdirectory(src/cmdlineoptions)
add_subdirectory(src/devicecommon)
add_subdirectory(src/fio)
add_subdirectory(src/diskdrive)
add_subdirectory(src/float)
add_subdirectory(src/bdv11)
add_subdirectory(src/m792)
add_subdirectory(src/m9312)
add_subdirectory(src/kt24)
add_subdirectory(src/kw11l)
add_subdirectory(src/chrono)
add_subdirectory(src/ba11)
add_subdirectory(src/proc)
add_subdirectory(src/rk)
add_subdirectory(src/rlv12)
add_subdirectory(src/bus)
add_subdirectory(src/configdata)
add_subdirectory(src/sdl)
add_subdirectory(src/trace)
add_subdirectory(tracereader_src)
add_subdirectory(src/console)
add_subdirectory(src/rxv21)
add_subdirectory(src/pdp11)
add_subdirectory(src/dl11)
add_subdirectory(src/logger)
add_subdirectory(src/asynctimer)
add_subdirectory(src/cabinet)
add_subdirectory(src/absoluteloader)
add_subdirectory(src/abstractbusdevice)
add_subdirectory(src/main)
add_subdirectory(src/mem)
add_subdirectory(src/signal)
add_subdirectory(src/imagedata)
add_subdirectory(src/wasm)
add_subdirectory(gtest)

install(TARGETS ${SIM11_TARGET}
    DESTINATION sim-11
)

install(DIRECTORY config resources
    DESTINATION sim-11
)