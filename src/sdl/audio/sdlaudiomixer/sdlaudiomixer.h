#ifndef _SDLAUDIOMIXER_H_
#define _SDLAUDIOMIXER_H_

#include "../include/audio.h"
#include "../sdlaudiodevice/sdlaudiodevice.h"
#include "../sdlaudioplayer/sdlaudioplayer.h"
#include "concepts//playlist/playlist.h"

#include <memory>
#include <string>
#include <vector>
#include <queue>

using std::unique_ptr;
using std::string;
using std::vector;
using std::queue;

class SDLAudioMixer 
{
public:
    class SDLChannel;

    void setOutputVolume (float volume);
    float volume ();
    void createChannel (AudioPlayer* audioPlayer);

    // fill is defined public to make it accessible to unit tests
    void fill (AudioFrame* stream, size_t numberOfFrames);

private:
    vector<AudioPlayer*> audioPlayers_ {};
    vector<AudioFrame> channelBuffer_;
    float volume_ {1.0};

    size_t mixChannels (AudioFrame* stream, size_t numberOfFrames);
    void adjustVolume (AudioFrame* stream, size_t numberOfFrames);
    size_t mixInto (const AudioFrame* source, AudioFrame* destination,
        size_t numberOfFrames);
};

#endif // _SDLAUDIOMIXER_H_