target_compile_definitions(${TARGET} PRIVATE
    CRT_SECURE_NO_WARNINGS
)

target_include_directories(${TARGET} PRIVATE
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include

    "${CMAKE_SOURCE_DIR}/contrib/termio/include"
    "${CMAKE_SOURCE_DIR}/contrib/clock_gettime/include"
    "${CMAKE_SOURCE_DIR}/contrib/sys_time_h"
    "${CMAKE_SOURCE_DIR}/contrib/unistd_h"
)

target_link_libraries(${TARGET} PRIVATE
    C:/Libraries/SDL2-2.24.2/lib/x64/SDL2.lib
    C:/Libraries/SDL2_image-2.6.2/lib/x64/SDL2_image.lib
)

# Unit-test target
set_property(TARGET ${SIM11_TEST_TARGET}
    PROPERTY MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_include_directories(${SIM11_TEST_TARGET} PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
    C:/Tools/gtest-1.16.0/include
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include
)

target_link_libraries(${SIM11_TEST_TARGET} PRIVATE
    C:/Tools/gtest-1.16.0/lib/gtest.lib
    C:/Libraries/SDL2-2.26.0/SDL2d.lib
    C:/Libraries/SDL2_image-2.6.2/lib/x64/SDL2_image.lib
)

enable_testing()

# Installation of the sim-11 target
#
# The binary, resources and config directories are all installed in the same
# destination directory. On Windows the SDL2 dll's are installed in that
# directory too.
#
# In Visual Studio 2022 the destination directory is
# CMAKE_SOURCE_DIR\out\install\<Configuration Name>\sim-11; on Linux the
# destination directory is /usr/local/sim-11.
#
install(FILES
    "${CMAKE_SOURCE_DIR}/externals/SDL2.dll"
    "${CMAKE_SOURCE_DIR}/externals/SDL2_image.dll"
    "${CMAKE_SOURCE_DIR}/externals/SDL2d.dll"
    DESTINATION sim-11
)