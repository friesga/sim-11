set (SDL_SRCS
    sdlinit/sdlinit.cpp
    datapanelbuilder/datapanelbuilder.cpp
    filepanelbuilder/filepanelbuilder.cpp
    sdlpanel/sdlpanel.cpp
    sdlwindow/sdlwindow.cpp
    sdlindicator/sdlindicator.cpp
    sdlfront/sdlfront.cpp
    sdlcommonbutton/sdlcommonbutton.cpp
    sdlrenderer/sdlrenderer.cpp
    sdlrenderer/circle.cpp
    sdltexture/sdltexture.cpp
    sdltile/sdltile.cpp
    sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp
    sdlnpositionswitch/sdlnpositionswitch.cpp
    sdlevent/sdlevent.cpp)

target_sources (${TARGET} PRIVATE ${SDL_SRCS})