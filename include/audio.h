#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "../audioframe/audioframe.h"

#include <cstdint>
#include <string>
#include <initializer_list>

using std::string;
using std::initializer_list;

enum class AudioSampleFormat
{
    UInt8,
    Int16,
    Int32,
    Float32
};

struct AudioFormat
{
    size_t sampleRateHz {};            // E.g. 44100 or 48000
    size_t numberOfChannels {};        // E.g. 1 for mono, 2 for stereo
    AudioSampleFormat sampleFormat {}; // E.g. UInt8, Int16, Int32, Float32
};

enum class PlaybackMode
{
    Continuous,
    OneShot
};

// This class represents an audiostream, originating from a WAV file.
class AudioStream
{
public:
    virtual ~AudioStream () = default;
    virtual bool eos () = 0;
    virtual void reset () = 0;
    virtual size_t fill (AudioFrame* stream, size_t numberOfFrames) = 0;
};

struct Track
{
    AudioStream* audioStream {nullptr};
    PlaybackMode playbackMode {PlaybackMode::Continuous};
};

class Playlist
{
public:
    virtual ~Playlist () = default;
    virtual void assignTrack (Track track) = 0;
    virtual void addTrack (Track track) = 0;
    virtual const Track* currentTrack () = 0;
    virtual const Track* nextTrack () = 0;
    virtual void clear () = 0;
};

class AudioMixer
{
public:
    class Channel;

    virtual ~AudioMixer () = default;
    virtual void start () = 0;
    virtual void stop () = 0;
    virtual bool isPlaying () = 0;
    virtual size_t createChannel (string label) = 0;
    virtual Channel& channel (size_t id) = 0;
    virtual void setVolume (float volume) = 0;
    virtual float volume () = 0;
};

class AudioMixer::Channel
{
public:
    virtual ~Channel () = default;

    virtual void play (AudioStream* stream) = 0;
    virtual void play (Track track) = 0;
    virtual void play (initializer_list<Track> tracks) = 0;
    virtual AudioStream* source () = 0;
    virtual void stop () = 0;
    virtual bool isPlaying () = 0;
    virtual void setVolume (float volume) = 0;
    virtual float volume () = 0;
    virtual size_t fill (AudioFrame* stream, size_t numberOfFrames) = 0;
};


#endif // _AUDIO_H_