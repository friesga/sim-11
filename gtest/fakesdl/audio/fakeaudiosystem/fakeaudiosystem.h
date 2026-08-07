#ifndef _FAKEAUDIOSYSTEM_H_
#define _FAKEAUDIOSYSTEM_H_

#include "audio.h"
#include "../fakeaudioplayer/fakeaudioplayer.h"

#include <memory>
#include <optional>
#include <string>

using std::unique_ptr;
using std::make_unique;
using std::optional;
using std::nullopt;
using std::string;

class FakeAudioSystem : public AudioSystem
{
public:
    // Function required by the AudioSystem interface
    AudioPlayer* createPlayer (optional<string> filename =  nullopt)
    {
        return audioPlayer_.get ();
    }

    AudioFormat audioFormat () const override
    {
        return AudioFormat {44100, 2, AudioSampleFormat::Int16};
    }

    // Functions required by the AudioControl interface
    void start () override {}
    void stop () override {}
    bool isPlaying () override { return false; }
    void setVolume (float volume) override {}
    float volume () override { return 0.0f; }

private:
    unique_ptr<FakeAudioPlayer> audioPlayer_ {make_unique<FakeAudioPlayer> ()};
};

#endif // _FAKEAUDIOSYSTEM_H_