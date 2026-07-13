#include <gtest/gtest.h>

#include "sdl/audio/sdlaudiostream/sdlaudiostream.h"
#include "sdl/audio/sdlaudiomixer/sdlaudiomixer.h"

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
};

bool floatEquals (float f1, float f2)
{
    return fabs (f1 - f2) < numeric_limits<float>::epsilon ();
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);
    EXPECT_EQ (audioMixer.createChannel (""), 1);

    audioMixer.channel (0).play (&audioStream);
    audioMixer.channel (1).play (&audioStream);

    audioMixer.start ();
    AudioFrame* receivedFrames = new AudioFrame[channelSamples.size ()];
    audioMixer.audioCallback (nullptr, receivedFrames, channelSamples.size ());
    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  0.40));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -0.40));

    delete[] receivedFrames;
}


// This test verifies that channels with different number of samples
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};

    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);
    EXPECT_EQ (audioMixer.createChannel (""), 1);

    audioMixer.channel (0).play (&audioStream0);
    audioMixer.channel (1).play (&audioStream1);

    audioMixer.start ();
    AudioFrame* receivedFrames = new AudioFrame[samplesSize];
    audioMixer.audioCallback (nullptr, receivedFrames, samplesSize);

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

    delete[] receivedFrames;
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};

    audioMixer.createChannel ("test");
    audioMixer.channel (0).play (&audioStream);
    AudioFrame* receivedFrames = new AudioFrame[samples.size ()];
    audioMixer.audioCallback (nullptr, receivedFrames, samples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.90));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.90));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -1.00));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -1.00));

    delete[] receivedFrames;
}

TEST_F (SDLAudioTest, audioMixerStreamIsClamped)
{
    vector<AudioFrame> samples
    {
        {0.40, -0.40},
        {0.50, -0.50},
        {0.60, -0.60},
    };

    SDLAudioStream audioStream0 {samples};

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);
    EXPECT_EQ (audioMixer.createChannel (""), 1);

    audioMixer.channel (0).play (&audioStream0);
    audioMixer.channel (1).play (&audioStream0);

    audioMixer.start ();
    AudioFrame* receivedFrames = new AudioFrame[samples.size ()];
    audioMixer.audioCallback (nullptr, receivedFrames, samples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (),  0.80));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), -0.80));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (),   1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -1.00));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),   1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (),  -1.00));

    delete[] receivedFrames;
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
    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    audioMixer.createChannel ("test");
    audioMixer.channel (0).play (&audioStream);
    audioMixer.setVolume (2.0f);

    AudioFrame* receivedFrames = new AudioFrame[samples.size ()];
    audioMixer.audioCallback (nullptr, receivedFrames, samples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left  (),  0.20));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.20));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left  (),  1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -1.00));

    delete[] receivedFrames;
}

TEST_F (SDLAudioTest, channelVolumeCanBeSet)
{
    vector<AudioFrame> samples
    {
        {0.0,   0.0},
        {0.10, -0.10},
        {0.60, -0.60},
    };

    SDLAudioStream audioStream0 {samples};

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);
    EXPECT_EQ (audioMixer.createChannel (""), 1);

    audioMixer.channel (0).play (&audioStream0);
    audioMixer.channel (1).play (&audioStream0);
    audioMixer.channel (0).setVolume (2.0f);

    audioMixer.start ();
    AudioFrame* receivedFrames = new AudioFrame[samples.size ()];
    audioMixer.audioCallback (nullptr, receivedFrames, samples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left  (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (),   0.30));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.30));

    EXPECT_TRUE (floatEquals (receivedFrames[2].left (),   1.00));
    EXPECT_TRUE (floatEquals (receivedFrames[2].right (), -1.00));

    delete[] receivedFrames;
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);

    audioMixer.channel (0).play (Track {&audioStream, PlaybackMode::OneShot});

    audioMixer.start ();

    const size_t receivedFramesSize = 10;
    AudioFrame* receivedFrames = new AudioFrame[10];

    for (size_t i = 0; i < receivedFramesSize; ++i)
        receivedFrames[i] = AudioFrame {0.0f, 0.0f};

    audioMixer.audioCallback (nullptr, receivedFrames, channelSamples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));

    delete[] receivedFrames;
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);

    audioMixer.channel (0).play (Track {&audioStream, PlaybackMode::OneShot});

    audioMixer.start ();

    const size_t receivedFramesSize = 10;
    AudioFrame* receivedFrames = new AudioFrame[10];

    for (size_t i = 0; i < receivedFramesSize; ++i)
        receivedFrames[i] = AudioFrame {0.0f, 0.0f};

    audioMixer.audioCallback (nullptr, receivedFrames, channelSamples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));

    // Repeat the one-shot playback
    for (size_t i = 0; i < receivedFramesSize; ++i)
        receivedFrames[i] = AudioFrame {0.0f, 0.0f};

    audioMixer.channel (0).play (Track {&audioStream, PlaybackMode::OneShot});

    audioMixer.audioCallback (nullptr, receivedFrames, channelSamples.size ());

    EXPECT_TRUE (floatEquals (receivedFrames[0].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[0].right (), 0.00));

    EXPECT_TRUE (floatEquals (receivedFrames[1].left (), 0.10));
    EXPECT_TRUE (floatEquals (receivedFrames[1].right (), -0.10));

    EXPECT_TRUE (floatEquals (receivedFrames[3].left (), 0.00));
    EXPECT_TRUE (floatEquals (receivedFrames[3].right (), -0.00));

    delete[] receivedFrames;
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);

    audioMixer.channel (0).play ({
        {&audioStream0, PlaybackMode::OneShot},
        {&audioStream1, PlaybackMode::OneShot}});

    const size_t receivedFramesSize = 10;
    AudioFrame* receivedFrames = new AudioFrame[10];

    for (size_t i = 0; i < receivedFramesSize; ++i)
        receivedFrames[i] = AudioFrame {0.0f, 0.0f};

    audioMixer.audioCallback (nullptr, receivedFrames, receivedFramesSize);

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

    delete[] receivedFrames;
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

    AudioFormat audioFormat {48000, 2, AudioSampleFormat::Float32};
    SDLAudioMixer audioMixer {audioFormat};
    EXPECT_EQ (audioMixer.createChannel (""), 0);

    audioMixer.channel (0).play ({
        {&audioStream0, PlaybackMode::OneShot},
        {&audioStream1, PlaybackMode::Continuous}});


    const size_t receivedFramesSize = 10;
    AudioFrame* receivedFrames = new AudioFrame[10];

    for (size_t i = 0; i < receivedFramesSize; ++i)
        receivedFrames[i] = AudioFrame {0.0f, 0.0f};

    audioMixer.audioCallback (nullptr, receivedFrames, receivedFramesSize);

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

    delete[] receivedFrames;
}
