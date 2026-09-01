set (SDL_VIDEO_SRCS
    datapanelbuilder/datapanelbuilder.cpp
    filepanelbuilder/filepanelbuilder.cpp
    sdlpanel/sdlpanel.cpp
    sdlwindow/sdlwindow.cpp
    sdlindicator/sdlindicator.cpp
    sdlfront/sdlfront.cpp
    sdlproperties/sdlproperties.cpp
    sdlrenderer/sdlrenderer.cpp
    sdlrenderer/circle.cpp
    sdltexture/sdltexture.cpp
    sdltile/sdltile.cpp
    sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp
    sdlmultipositionswitch/sdlmultipositionswitch.cpp
    sdlevent/sdlevent.cpp)
target_sources (${TARGET} PRIVATE ${SDL_VIDEO_SRCS})
target_sources (sim-11-wasm PRIVATE ${SDL_VIDEO_SRCS})