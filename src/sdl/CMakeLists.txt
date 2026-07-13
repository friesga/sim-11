set (SDL_SRCS
    sdlinit/sdlinit.cpp
    video/datapanelbuilder/datapanelbuilder.cpp
    video/filepanelbuilder/filepanelbuilder.cpp
    video/sdlpanel/sdlpanel.cpp
    video/sdlwindow/sdlwindow.cpp
    video/sdlindicator/sdlindicator.cpp
    video/sdlfront/sdlfront.cpp
    video/sdlrenderer/sdlrenderer.cpp
    video/sdlrenderer/circle.cpp
    video/sdltexture/sdltexture.cpp
    video/sdltile/sdltile.cpp
    video/sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp
    video/sdlmultipositionswitch/sdlmultipositionswitch.cpp
    video/sdlevent/sdlevent.cpp)
target_sources (${TARGET} PRIVATE ${SDL_SRCS})
target_sources (sim-11-wasm PRIVATE ${SDL_SRCS})