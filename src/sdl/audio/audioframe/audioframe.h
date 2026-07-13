#ifndef _AUDIOFRAME_H_
#define _AUDIOFRAME_H_

#include <type_traits>

// This class contains the audio data for a single frame of stereo audio,
// plus the necessary function to process the data. A frame contains 
// one sample for the left and one sample for the right channel. 
// The samples are represented as floating-point values to faciltiate audio
// processing and mixing. 
class AudioFrame
{
public:
    AudioFrame ();
    AudioFrame (float left, float right);

    AudioFrame& operator+= (const AudioFrame& other);
    AudioFrame& operator*= (float factor) noexcept;

    AudioFrame clampVolume ();
    float left ();
    float right ();

private:
    float left_ {0.0f};
    float right_ {0.0f};
};

inline AudioFrame& AudioFrame::operator*= (float factor) noexcept
{
    left_  *= factor;
    right_ *= factor;
    return *this;
}

inline AudioFrame operator* (AudioFrame frame, float factor) noexcept
{
    frame *= factor;
    return frame;
}

inline AudioFrame operator* (float factor, AudioFrame frame) noexcept
{
    frame *= factor;
    return frame;
}

// Verify that a AudioFrame can be casted to an audiostream consisting
// of two floats (one for the left and one for the right channel).
static_assert (sizeof (AudioFrame) == sizeof (float) * 2,
    "AudioFrame must contain exactly two floats");

static_assert (std::is_trivially_copyable_v<AudioFrame>,
    "AudioFrame must be trivially copyable");

static_assert (std::is_standard_layout_v<AudioFrame>,
    "AudioFrame must have standard layout");

#endif // _AUDIOFRAME_H_