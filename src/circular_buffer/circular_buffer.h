#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include "conddata/conddata.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

// Implementation of a circular buffer, derived from 
// https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
// as published on 
// https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer/circular_buffer.hpp
// 
// Licensed under the Creative Commons Zero v1.0 Universal license.
// 
// Note this implementation if not thread-safe. A lock is needed in size()
// to prevent a race condition, because head_, tail_, and full_ can be updated
// between executing lines within this function in a multi-threaded
// application.
//
// Changes to the original source:
// - Renamed circular_buffer to CircularBuffer and TElemCount to SIZE,,
// - Removed lock guards as the operations will be part of locked regions
//   (and thread safety could be provided by a subclassed implementation),
// - Reformatted code,
// - Replaced std::optional by CondData,
// - Added overflow detection,
// - Allocate buffer on heap.
//
template<class T, size_t SIZE>
class CircularBuffer
{
public:
    CircularBuffer ();
    bool put (T item) noexcept;
    CondData<T> get () noexcept;
    void reset () noexcept;
    bool empty () const noexcept;
    bool full () const noexcept;
    size_t capacity () const noexcept;
    size_t size () const noexcept;

private:
    unique_ptr<T[]> buf_;
    size_t head_ = 0;
    size_t tail_ = 0;
    bool full_ = 0;
};

template<class T, size_t SIZE>
CircularBuffer<T, SIZE>::CircularBuffer ()
    :
    buf_ {make_unique<T[]> (SIZE)}
{}

template<class T, size_t SIZE>
bool CircularBuffer<T, SIZE>::put (T item) noexcept
{
    if (full_)
        return false;

    buf_[head_] = item;
    head_ = (head_ + 1) % SIZE;
    full_ = head_ == tail_;

    return true;
}

template<class T, size_t SIZE>
CondData<T> CircularBuffer<T, SIZE>::get () noexcept
{
    if (empty ())
        return {};

    // Read data and advance the tail (we now have a free space)
    auto val = buf_[tail_];
    full_ = false;
    tail_ = (tail_ + 1) % SIZE;

    return val;
}

template<class T, size_t SIZE>
void CircularBuffer<T, SIZE>::reset () noexcept
{
    head_ = tail_;
    full_ = false;
}

template<class T, size_t SIZE>
bool CircularBuffer<T, SIZE>::empty () const noexcept
{
    // If head and tail are equal, the buffer is empty
    return (!full_ && (head_ == tail_));
}

template<class T, size_t SIZE>
bool CircularBuffer<T, SIZE>::full () const noexcept
{
    // If tail is ahead the head by one, the buffer is full
    return full_;
}

template<class T, size_t SIZE>
size_t CircularBuffer<T, SIZE>::capacity () const noexcept
{
    return SIZE;
}

template<class T, size_t SIZE>
size_t CircularBuffer<T, SIZE>::size () const noexcept
{
    size_t size = SIZE;

    if (!full_)
    {
        if (head_ >= tail_)
            size = head_ - tail_;
        else
            size = SIZE + head_ - tail_;
    }

    return size;
}

#endif // _CIRCULAR_BUFFER_H_
