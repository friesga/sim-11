#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "concepts/audioframe/audioframe.h"
#include "concepts/playlist/playlist.h"

#include <cstdint>
#include <string>
#include <initializer_list>
#include <optional>

using std::string;
using std::initializer_list;
using std::optional;
using std::nullopt;

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

// This class represents an audiostream, originating from a WAV file.
class AudioStream
{
public:
    virtual ~AudioStream () = default;
    virtual bool eos () = 0;
    virtual void reset () = 0;
    virtual size_t fill (AudioFrame* stream, size_t numberOfFrames) = 0;
};

enum class PlaybackMode
{
    Continuous,
    OneShot
};

struct Track
{
    AudioStream* audioStream {nullptr};
    PlaybackMode playbackMode {PlaybackMode::Continuous};
};

struct FileTrack
{
    string filename {};
    PlaybackMode playbackMode {PlaybackMode::Continuous};
};

class AudioControl
{
public:
    virtual void start () = 0;
    virtual void stop () = 0;
    virtual bool isPlaying () = 0;
    virtual void setVolume (float volume) = 0;
    virtual float volume () = 0;
};

class AudioPlayer : public AudioControl
{
public:
    virtual ~AudioPlayer () = default;
    virtual void play (AudioStream* stream,
        PlaybackMode playbackMode = PlaybackMode::Continuous) = 0;
    virtual void play (Playlist<Track> playlist) = 0;
    virtual void play (string filename,
        PlaybackMode playbackMode = PlaybackMode::Continuous) = 0;
    virtual void play (Playlist<FileTrack>) = 0;
    virtual size_t fill (AudioFrame* stream, size_t numberOfFrames) = 0;
};

class AudioSystem : public AudioControl
{
public:
    virtual AudioPlayer* createPlayer (optional<string> filename =  nullopt) = 0;
    virtual AudioFormat audioFormat () const = 0;
};

#endif // _AUDIO_H_