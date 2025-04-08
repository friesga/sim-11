set (SDL_SRCS
    sdlinit/sdlinit.cpp
    sdlpanel/sdlpanel.cpp
    sdlwindow/sdlwindow.cpp
    sdlwindow/circle.cpp
    sdlindicator/sdlindicator.cpp
    sdlfront/sdlfront.cpp
    sdlcommonbutton/sdlcommonbutton.cpp
    sdlrenderer/sdlrenderer.cpp
    sdltexture/sdltexture.cpp
    sdlmomentarybutton/sdlmomentarybutton.cpp
    sdllatchingbutton/sdllatchingbutton.cpp
    sdlindicatorlatchingbutton/sdlindicatorlatchingbutton.cpp
    sdlthreepositionswitch/sdlthreepositionswitch.cpp
    sdlfourpositionswitch/sdlfourpositionswitch.cpp
    sdlevent/sdlevent.cpp)

target_sources (${TARGET} PRIVATE ${SDL_SRCS})