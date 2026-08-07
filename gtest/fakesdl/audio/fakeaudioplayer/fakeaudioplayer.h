#ifndef _FAKEAUDIOPLAYER_H_
#define _FAKEAUDIOPLAYER_H_

#include "audio.h"

#include <string>

using std::string;

class FakeAudioPlayer : public AudioPlayer
{
public:
    // Functions required by the AudioPlayer interface
    void play (AudioStream* stream,
        PlaybackMode playbackMode = PlaybackMode::Continuous) override
    {}

    void play (Playlist<Track> playlist) override
    {}

    void play (string filename,
        PlaybackMode playbackMode = PlaybackMode::Continuous) override
    {}

    void play (Playlist<FileTrack> fileTracks) override
    {}

    bool isPlaying () override 
    {
        return false;
    }

    void start () override {}
    void stop () override {}
    void setVolume (float volume) override {}
    float volume () override { return 0.0f; }   
    
    size_t fill (AudioFrame* stream, size_t numberOfFrames) override
    {
        return 0;
    }
};


#endif // _FAKEAUDIOPLAYER_H_