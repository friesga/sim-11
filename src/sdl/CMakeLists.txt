set (SDL_SRCS
    sdlinit/sdlinit.cpp
    audio/audioframe/audioframe.cpp
    audio/sdlaudiodevice/sdlaudiodevice.cpp
    audio/sdlaudiodevicelock/sdlaudiodevicelock.cpp
    audio/sdlaudiospec/sdlaudiospec.cpp
    audio/sdlaudiostream/sdlaudiostream.cpp
    audio/sdlaudiomixer/sdlaudiomixer.cpp
    audio/sdlaudiomixer/sdlchannel.cpp
    audio/sdlaudiomixer/playlist.cpp
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
target_sources (sim-11-test PRIVATE ${SDL_SRCS})
target_sources (sim-11-wasm PRIVATE ${SDL_SRCS})