target_compile_definitions(${TARGET} PRIVATE
    CRT_SECURE_NO_WARNINGS
)

target_include_directories(${TARGET} PRIVATE
    /usr/include/SDL2

    "${CMAKE_SOURCE_DIR}/contrib/termio/include"
    "${CMAKE_SOURCE_DIR}/contrib/clock_gettime/include"
    "${CMAKE_SOURCE_DIR}/contrib/sys_time_h"
    "${CMAKE_SOURCE_DIR}/contrib/unistd_h"
)

target_link_options (${TARGET} PRIVATE
    -pthread -Wl,-x -Wl,--gc-sections
)

target_link_libraries(${TARGET} PRIVATE
    SDL2
    SDL2_image
)

# Unit-test target
set_property(TARGET ${SIM11_TEST_TARGET}
    PROPERTY MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_include_directories(${SIM11_TEST_TARGET} PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
    /usr/include/SDL2
)

target_link_libraries(${SIM11_TEST_TARGET} PRIVATE
    SDL2
    SDL2_image
)

enable_testing()

install(FILES
    "${CMAKE_SOURCE_DIR}/externals/SDL2.dll"
    "${CMAKE_SOURCE_DIR}/externals/SDL2_image.dll"
    "${CMAKE_SOURCE_DIR}/externals/SDL2d.dll"
    DESTINATION sim-11
)