set (SDL_INIT_SRCS
    sdlinit.cpp)

target_sources (${TARGET} PRIVATE ${SDL_INIT_SRCS})
target_sources (sim-11-test PRIVATE ${SDL_INIT_SRCS})
target_sources (sim-11-wasm PRIVATE ${SDL_INIT_SRCS})