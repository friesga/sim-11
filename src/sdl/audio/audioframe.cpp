#include "audioframe.h"

#include <algorithm>
 
AudioFrame::AudioFrame ()
    :
    left_ {0.0},
    right_ {0.0}
{}

AudioFrame::AudioFrame (float left, float right)
    :
    left_ {left},
    right_ {right}
{}

AudioFrame& AudioFrame::operator+= (const AudioFrame& other)
{
    left_  += other.left_;
    right_ += other.right_;
    return *this;
}

AudioFrame AudioFrame::clampVolume ()
{
    return {std::clamp (left_,  -1.0f, 1.0f),
            std::clamp (right_, -1.0f, 1.0f)};
}

float AudioFrame::left ()
{
    return left_;
}

float AudioFrame::right ()
{
    return right_;
}

