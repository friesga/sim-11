#include <gtest/gtest.h>

#include "sdl/audio/sdlaudiostream/sdlaudiostream.h"
#include "sdl/audio/sdlaudiosystem/sdlaudiosystem.h"
#include "sdl/audio/sdlaudioplayer/sdlaudioplayer.h" 

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

using std::vector;
using std::numeric_limits;
using std::max;

// Definition of a test fixture to initialize SDL audio.
class SDLAudioTest : public ::testing::Test
{
public:
    SDLAudioTest ()
    {
        SDL_Init (SDL_INIT_AUDIO);
    }

protected:
    SDL_AudioSpec spec
    {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = 44100
    };

    SDL_AudioStream* sdlStream =
        SDL_CreateAudioStream (&spec, &spec);
};

bool floatEquals (float f1, float f2)
{
    return fabs (f1 - f2) < numeric_limits<float>::epsilon ();
}

TEST_F (SDLAudioTest, audioStreamCanBePlayed)
{
    vector<AudioFrame> channelSamples
    {
        {0.00, 0.00},
        {0.10, -0.10},
        {0.20, -0.20}
    };

    SDLAudioStream audioStream {channelSamples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play (&audioStream);

    audioSystem.audioCallback (nullptr, sdlStream, channelSamples.size ());

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (channelSamples.size ());
    const int receivedBytes = SDL_GetAudioStreamData (
            sdlStream,
            receivedFrames.data (),
            channelSamples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left (), 0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.20));
}


TEST_F (SDLAudioTest, audioCanBeMixed)
{
    vector<AudioFrame> channelSamples
    {
        {0.00, 0.00},
        {0.10, -0.10},
        {0.20, -0.20}
    };

    SDLAudioStream audioStream {channelSamples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer0 = audioSystem.createPlayer ();
    AudioPlayer* audioPlayer1 = audioSystem.createPlayer ();

    audioPlayer0->play (&audioStream);
    audioPlayer1->play (&audioStream);

    audioSystem.audioCallback (nullptr, sdlStream, channelSamples.size ());

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (channelSamples.size ());
    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        receivedFrames.data (),
        channelSamples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.40));
}


// This test verifies that streams with different number of samples
// can be played continously.
TEST_F (SDLAudioTest, differentChanelsCanBeMixed)
{
    vector<AudioFrame> channel0Samples
    {
        {0.00,  0.00},
        {0.10, -0.10},
        {0.20, -0.20},
        {0.30, -0.30},
        {0.40, -0.40},
        {0.50, -0.50}
    };

    vector<AudioFrame> channel1Samples
    {
        {0.00,  0.00},
        {0.10, -0.10},
        {0.20, -0.20}
    };

    const size_t samplesSize = max (channel0Samples.size (),
        channel1Samples.size ());

    SDLAudioStream audioStream0 {channel0Samples};
    SDLAudioStream audioStream1 {channel1Samples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer0 = audioSystem.createPlayer ();
    AudioPlayer* audioPlayer1 = audioSystem.createPlayer ();

    audioPlayer0->play (&audioStream0);
    audioPlayer1->play (&audioStream1);

    audioSystem.audioCallback (nullptr, sdlStream, samplesSize);

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (samplesSize);
    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        receivedFrames.data (),
        samplesSize * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.40));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (),   0.30));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.30));

    EXPECT_TRUE (floatEquals (receivedFrames[4].left (), 0.50));
    EXPECT_TRUE (floatEquals (receivedFrames[4].right (), -0.50));
}


TEST_F (SDLAudioTest, streamIsClamped)
{
    vector<AudioFrame> samples
    {
        {0.90, -0.90},
        {1.00, -1.00},
        {1.20, -1.20}
    };

    SDLAudioStream audioStream {samples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play (&audioStream);

    audioSystem.audioCallback (nullptr, sdlStream, samples.size ());

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (samples.size ());
    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        receivedFrames.data (),
        samples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.90));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.90));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -1.00));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -1.00));
}


TEST_F (SDLAudioTest, audioMixerStreamIsClamped)
{
    vector<AudioFrame> samples
    {
        {0.40, -0.40},
        {0.50, -0.50},
        {0.60, -0.60},
    };

    SDLAudioStream audioStream {samples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer0 = audioSystem.createPlayer ();
    AudioPlayer* audioPlayer1 = audioSystem.createPlayer ();

    audioPlayer0->play (&audioStream);
    audioPlayer1->play (&audioStream);

    audioSystem.audioCallback (nullptr, sdlStream, samples.size ());

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (samples.size ());
    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        receivedFrames.data (),
        samples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.80));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.80));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (),   1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -1.00));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),   1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (),  -1.00));
}


TEST_F (SDLAudioTest, audioPlayerVolumeCanBeSet)
{
    vector<AudioFrame> samples
    {
        {0.00,  0.00},
        {0.10, -0.10},
        {0.60, -0.60}
    };

    SDLAudioStream audioStream {samples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play (&audioStream);
    audioPlayer->setVolume (2.0f);

    audioSystem.audioCallback (nullptr, sdlStream, samples.size ());

    // Get the AudioFrames from the AudioStream
    vector<AudioFrame> receivedFrames (samples.size ());
    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        receivedFrames.data (),
        samples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -1.00));
}


// Verify the audio is played once when the channel is set to
// one-shot mode, and that the audio is not repeated.
TEST_F (SDLAudioTest, audioCanBePlayedInOneShotMode)
{
    vector<AudioFrame> channelSamples
    {
        {0.00, 0.00},
        {0.10, -0.10},
        {0.20, -0.20}
    };

    SDLAudioStream audioStream {channelSamples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play (&audioStream, PlaybackMode::OneShot);

    audioSystem.audioCallback (nullptr, sdlStream, channelSamples.size ());

    // Get the AudioFrames from the AudioStream
    // The test verifies that the frames after the provided samples are zero
    // and therefore the receivedFrames buffer must be larger than the
    // channelSamples.size ().
    vector<AudioFrame> receivedFrames (channelSamples.size () + 1,
        AudioFrame {0.0f, 0.0f});

    const int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        static_cast<void*> (receivedFrames.data ()),
        channelSamples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));
}


TEST_F (SDLAudioTest, oneShotModeCanBeRepeated)
{
    vector<AudioFrame> channelSamples
    {
        {0.00, 0.00},
        {0.10, -0.10},
        {0.20, -0.20}
    };

    SDLAudioStream audioStream {channelSamples};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play (&audioStream, PlaybackMode::OneShot);

    audioSystem.audioCallback (nullptr, sdlStream, channelSamples.size ());

    // Get the AudioFrames from the AudioStream
    // The test verifies that the frames after the provided samples are zero
    // and therefore the receivedFrames buffer must be larger than the
    // channelSamples.size ().
    vector<AudioFrame> receivedFrames (10, AudioFrame {0.0f, 0.0f});

    int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        static_cast<void*> (receivedFrames.data ()),
        channelSamples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));

    // Repeat the one-shot playback
    audioPlayer->play (&audioStream, PlaybackMode::OneShot);

    audioSystem.audioCallback (nullptr, sdlStream, channelSamples.size ());

    for (AudioFrame audioFrame : channelSamples)
        audioFrame = AudioFrame {0.0f, 0.0f};

    receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        static_cast<void*> (receivedFrames.data ()),
        channelSamples.size () * sizeof (AudioFrame));

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));
}


TEST_F (SDLAudioTest, listOfTracksCanBePlayed)
{
    vector<AudioFrame> channelSamples0
    {
        {0.10, -0.10},
        {0.20, -0.20},
        {0.30, -0.30},
    };

    vector<AudioFrame> channelSamples1
    {
        {0.40, -0.40},
        {0.50, -0.50},
        {0.60, -0.60},
    };

    SDLAudioStream audioStream0 {channelSamples0};
    SDLAudioStream audioStream1 {channelSamples1};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play ({
        {&audioStream0, PlaybackMode::OneShot},
        {&audioStream1, PlaybackMode::OneShot}});

    const size_t receivedFramesSize = 10;

    audioSystem.audioCallback (nullptr, sdlStream, receivedFramesSize);

    vector<AudioFrame> receivedFrames (receivedFramesSize, AudioFrame {0.0f, 0.0f});

    int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        static_cast<void*> (receivedFrames.data ()),
        receivedFramesSize * sizeof AudioFrame);

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  0.30));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.30));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.40));

    EXPECT_TRUE (floatEquals (receivedFrames[4].left  (),  0.50));
    EXPECT_TRUE (floatEquals (receivedFrames[4].right (), -0.50));  

    EXPECT_TRUE (floatEquals (receivedFrames[5].left  (),  0.60));
    EXPECT_TRUE (floatEquals (receivedFrames[5].right (), -0.60));

    EXPECT_TRUE (floatEquals (receivedFrames[6].left  (),  0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[6].right (),  0.00));
}


TEST_F (SDLAudioTest, oneShotFollowedByContinuousMode)
{
    vector<AudioFrame> channelSamples0
    {
        {0.10, -0.10},
        {0.20, -0.20},
        {0.30, -0.30},
    };

    vector<AudioFrame> channelSamples1
    {
        {0.40, -0.40},
        {0.50, -0.50},
        {0.60, -0.60},
    };

    SDLAudioStream audioStream0 {channelSamples0};
    SDLAudioStream audioStream1 {channelSamples1};

    SDLAudioSystem audioSystem;
    AudioPlayer* audioPlayer = audioSystem.createPlayer ();

    audioPlayer->play ({
        {&audioStream0, PlaybackMode::OneShot},
        {&audioStream1, PlaybackMode::Continuous}});

    const size_t receivedFramesSize = 10;

    audioSystem.audioCallback (nullptr, sdlStream, receivedFramesSize);

    vector<AudioFrame> receivedFrames (receivedFramesSize, AudioFrame {0.0f, 0.0f});

    int receivedBytes = SDL_GetAudioStreamData (
        sdlStream,
        static_cast<void*> (receivedFrames.data ()),
        receivedFramesSize * sizeof AudioFrame);

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  0.30));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.30));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.40));

    EXPECT_TRUE (floatEquals (receivedFrames[4].left  (),  0.50));
    EXPECT_TRUE (floatEquals (receivedFrames[4].right (), -0.50));

    EXPECT_TRUE (floatEquals (receivedFrames[5].left  (),  0.60));
    EXPECT_TRUE (floatEquals (receivedFrames[5].right (), -0.60));

    EXPECT_TRUE (floatEquals (receivedFrames[6].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[6].right (), -0.40));

    EXPECT_TRUE (floatEquals (receivedFrames[7].left  (),  0.50));
    EXPECT_TRUE (floatEquals (receivedFrames[7].right (), -0.50));

    EXPECT_TRUE (floatEquals (receivedFrames[8].left  (),  0.60));
    EXPECT_TRUE (floatEquals (receivedFrames[8].right (), -0.60));
}
