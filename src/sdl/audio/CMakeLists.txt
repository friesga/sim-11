set (SDL_AUDIO_SRCS
    audioframe/audioframe.cpp
    sdlaudiodevice/sdlaudiodevice.cpp
    sdlaudiodevicelock/sdlaudiodevicelock.cpp
    sdlaudiospec/sdlaudiospec.cpp
    sdlaudiostream/sdlaudiostream.cpp
    sdlaudiomixer/sdlaudiomixer.cpp
    sdlaudiosystem/sdlaudiosystem.cpp
    sdlaudioplayer/sdlaudioplayer.cpp
    playlist/playlist.cpp)

target_sources (${TARGET} PRIVATE ${SDL_AUDIO_SRCS})
target_sources (sim-11-test PRIVATE ${SDL_AUDIO_SRCS})
target_sources (sim-11-wasm PRIVATE ${SDL_AUDIO_SRCS})