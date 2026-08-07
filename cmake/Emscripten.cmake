set(SIM11_OUTPUT_NAME
    "sim-11-07be37664a"
    CACHE STRING
    "Base name of the generated WebAssembly files"
)

target_include_directories(${SIM11_WASM_TARGET} PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
    C:/Tools/emsdk-5.0.1/upstream/emscripten/cache/sysroot/include/emscripten
    C:/Libraries/SDL2-2.24.2/include
    C:/Libraries/SDL2_image-2.6.2/include
)

# Warnings about unqualified std::move calls are suppressed as Clang
# detects these incorrectly. See https://reviews.llvm.org/D119670?id=408276
# The same holds for warnings about self-assigning fields, warnings about
# undefined inline functions, warnings about braced scalar initializers
# and so on...
target_compile_options(${SIM11_WASM_TARGET} PRIVATE
    -fexceptions
    -pthread
    -O2
    -Wno-unqualified-std-cast-call
    -Wno-self-assign-field
    -Wno-undefined-inline
    -Wno-braced-scalar-init
    -Wno-uninitialized
)

# The SHELL: prefix is necessary to prevent CMake from combining the
# --preload-file options into a single option with a comma-separated list of
# files.
target_link_options(${SIM11_WASM_TARGET} PRIVATE
    -O2
    -fexceptions
    -pthread

    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sALLOW_MEMORY_GROWTH=1
    -sFORCE_FILESYSTEM
    "-sSDL2_IMAGE_FORMATS=[\"png\"]"
    -sEXPORTED_FUNCTIONS=_main,_on_xterm_char
    -sPTHREAD_POOL_SIZE_STRICT=0

    --shell-file
    "${CMAKE_SOURCE_DIR}/src/sim-11-shell.html"

    # The config and systems directories must not be included in the preloaded
    # filesystem for the sim-11.net WebAssembly build as the website  will
    # include a machine-specific filesystem.
    "SHELL:--preload-file ${CMAKE_BINARY_DIR}/resources@/resources"
    # "SHELL:--preload-file ${CMAKE_BINARY_DIR}/config@/config"
    # "SHELL:--preload-file ${CMAKE_BINARY_DIR}/systems@/systems"
)

set_property(TARGET ${SIM11_WASM_TARGET} APPEND PROPERTY
    LINK_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/sim-11-shell.html"
        "${CMAKE_BINARY_DIR}/config/pdp-11_03.ini"
        "${CMAKE_BINARY_DIR}/config/pdp-11_20.ini"
)

set_target_properties(${SIM11_WASM_TARGET} PROPERTIES
    OUTPUT_NAME "${SIM11_OUTPUT_NAME}"
    SUFFIX ".html"
)

# Copy the result of the build to the installation directory for use in 
# the website and copy the generated sim-11.html file to a stable name for
# testing purposes.
add_custom_command(TARGET ${SIM11_WASM_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE_DIR:${SIM11_WASM_TARGET}>/${SIM11_OUTPUT_NAME}.js"
        "$<TARGET_FILE_DIR:${SIM11_WASM_TARGET}>/${SIM11_OUTPUT_NAME}.wasm"
        "$<TARGET_FILE_DIR:${SIM11_WASM_TARGET}>/${SIM11_OUTPUT_NAME}.data"
        "${CMAKE_INSTALL_PREFIX}"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:${SIM11_WASM_TARGET}>"
        "$<TARGET_FILE_DIR:${SIM11_WASM_TARGET}>/sim-11.html"
)

# This target is used to clean up the sim-11 WebAssembly build products.
# The clean actions are performed in a seperate script so the files are
# searched and removed in the execution phase rather than in the
# configuration phase.
add_custom_target(clean-sim11
    COMMAND "${CMAKE_COMMAND}"
        "-DBUILD_DIRECTORY=$<TARGET_FILE_DIR:sim-11-wasm>"
        -P "${CMAKE_SOURCE_DIR}/cmake/clean-sim-11-wasm.cmake"

    COMMENT "Removing all sim-11 WebAssembly build products"
    VERBATIM
)