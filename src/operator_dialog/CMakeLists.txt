set (DIALOG_SDL_SOURCES
    SDL/sdlcartridge_handler/sdlcartridge_handler.cpp
    SDL/sdlfiledialog/sdlfiledialog.cpp
    SDL/sdlmessagebox/sdlmessagebox.cpp
    SDL/sdlpopupmenu/sdlpopupmenu.cpp
    SDL/sdlpopupmenu/sdlpopupmenusession.cpp)

set (DIALOG_WASM_SOURCES
    wasm/wasmcartridge_handler/wasmcartridge_handler.cpp
    wasm/wasmfiledialog/wasmfiledialog.cpp
    wasm/wasmmessagebox/wasmmessagebox.cpp
    wasm/wasmpopupmenu/wasmpopupmenu.cpp)

target_sources (${TARGET} PRIVATE ${DIALOG_SDL_SOURCES})
target_sources (sim-11-wasm PRIVATE ${DIALOG_WASM_SOURCES})
target_sources (sim-11-test PRIVATE ${DIALOG_SDL_SOURCES})