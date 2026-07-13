#ifndef _SDLAUDIOSTREAM_H_
#define _SDLAUDIOSTREAM_H_

#include "../include/audio.h"

#include <SDL.h>
#include <vector>

using std::vector;

class SDLAudioStream : public AudioStream
{
public:
    SDLAudioStream (const char* filename, const AudioFormat& targetSpec);
    SDLAudioStream (vector<AudioFrame> frames);

    // Functions required by the AudioStream interface
    bool eos () override;
    void reset () override;
    size_t fill (AudioFrame* stream, size_t numberOfFrames) override;

private:
    vector<AudioFrame> samples_;
    size_t playPosition_ {0};

    void loadWAVFile (const char* filename, const AudioFormat& targetSpec);
};


#endif // _SDLAUDIOSTREAM_H_