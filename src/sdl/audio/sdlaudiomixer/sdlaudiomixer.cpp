#include "sdlaudiomixer.h"
#include "../sdlaudiodevicelock/sdlauiodevicelock.h"

#include <algorithm>

using std::make_unique;
using std::max;

SDLAudioMixer::SDLAudioMixer (const AudioFormat& desiredAudioFormat)
    :
    audioDevice_ (desiredAudioFormat, [] (void* userdata, Uint8* stream, int length) {
    static_cast<SDLAudioMixer*>(userdata)->audioCallback (userdata,
        reinterpret_cast<AudioFrame*> (stream),
        length / static_cast<int>(sizeof (AudioFrame)));
        }, this)
{ }

void SDLAudioMixer::start ()
{
    audioDevice_.start ();
}

void SDLAudioMixer::stop ()
{
    audioDevice_.pause ();
}

bool SDLAudioMixer::isPlaying ()
{
    return audioDevice_.isPlaying ();
}

void SDLAudioMixer::audioCallback (void* userdata, AudioFrame* stream,
    size_t numberOfFrames)
{
    SDLAudioDeviceLock lock {audioDevice_};

    // First fill the stream with silence so the channel audio can be added
    // to the AudioFrames in the stream
    std::fill (stream, stream + numberOfFrames, AudioFrame {0.0f, 0.0f});

    size_t framesFilled = fill (stream, numberOfFrames);

    adjustVolume (stream, framesFilled);
}

size_t SDLAudioMixer::fill (AudioFrame* stream, size_t numberOfFrames)
{
    size_t numFramesInStream {0};

    channelBuffer_.resize (numberOfFrames);

    // For every available channel read the contents of that channel into
    // a buffer and add that channel into the mix in the output stream.
    // Keep track of the maximum number of AudioFrames put into the stream.
    for (const auto& channel : channels_)
    {
        // At this point possibly the channelBuffer should be cleared, but
        // as we keep track of the number of frames filled that should not
        // be necessary.
        // 
        // Disclaimer: the following three statements could be merged into
        // one, thereby removing the necessity for a intermediate variable
        // but this would obfuscate the intention of the code.
        //
        const size_t numFramesInChannel =  channel->fill (channelBuffer_.data (),
            numberOfFrames);

        mixInto (channelBuffer_.data (), stream, numFramesInChannel,
            channel->volume ());

        numFramesInStream = max (numFramesInStream, numFramesInChannel);
    }

    return numFramesInStream;
}

void SDLAudioMixer::adjustVolume (AudioFrame* stream, size_t numberOfFrames)
{
    for (size_t frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex)
        stream[frameIndex] = (stream[frameIndex] * volume_).clampVolume ();
}

size_t SDLAudioMixer::createChannel (string label)
{
    channels_.push_back (make_unique<SDLChannel> (label));
    return channels_.size () - 1;
}

SDLAudioMixer::Channel& SDLAudioMixer::channel (size_t id)
{
    return *channels_[id];
}

size_t SDLAudioMixer::mixInto (const AudioFrame* source, AudioFrame* destination,
    size_t numberOfFrames, float volume)
{
    for (size_t index = 0; index < numberOfFrames; ++index)
        destination[index] += source[index] * volume;

    return numberOfFrames;
}

void SDLAudioMixer::setVolume (float volume)
{
    volume_ = volume;
}

float SDLAudioMixer::volume ()
{
    return volume_;
}

AudioFormat SDLAudioMixer::audioFormat () const
{
    return audioDevice_.audioFormat ();
}

