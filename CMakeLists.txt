cmake_minimum_required(VERSION 3.16)

project(sim-11 LANGUAGES CXX)

include(GoogleTest)

option(BUILD_WITH_ASAN "Build with Address Sanitizer" OFF)

set(TARGET sim-11)
set(SIM11_TEST_TARGET sim-11-test)
set(SIM11_WASM_TARGET sim-11-wasm)
set(TRACEREADER_TARGET tracereader)

add_executable(${TARGET})
add_executable(${SIM11_TEST_TARGET})
add_executable(${SIM11_WASM_TARGET})
add_executable(${TRACEREADER_TARGET})

set_target_properties(${SIM11_TEST_TARGET} PROPERTIES
    EXCLUDE_FROM_ALL TRUE
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Common.cmake")

if(EMSCRIPTEN)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Emscripten.cmake")
elseif(WIN32)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Windows.cmake")
elseif(UNIX)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Linux.cmake")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()