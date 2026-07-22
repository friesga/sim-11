#ifndef _SDLAUDIOSTREAM_H_
#define _SDLAUDIOSTREAM_H_

#include "../include/audio.h"

#include <SDL.h>
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class SDLAudioStream : public AudioStream
{
public:
    SDLAudioStream (const char* filename, const AudioFormat& targetSpec);
    SDLAudioStream (vector<AudioFrame> frames);
    SDLAudioStream (vector<uint8_t>& wavData, const AudioFormat& targetSpec);

    // Functions required by the AudioStream interface
    bool eos () override;
    void reset () override;
    size_t fill (AudioFrame* stream, size_t numberOfFrames) override;

private:
    using WAVBufferPtr = std::unique_ptr<Uint8, decltype (&SDL_FreeWAV)>;

    vector<AudioFrame> samples_;
    size_t playPosition_ {0};

    void loadWAVFile (const char* filename, const AudioFormat& audioFormat);
    void convertWAVdata (WAVBufferPtr& wavBuffer, Uint32 bufferSize,
        SDL_AudioSpec fromSpec, SDL_AudioSpec toSpec);
};


#endif // _SDLAUDIOSTREAM_H_