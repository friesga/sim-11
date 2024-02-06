#ifndef _CHARACTERBUFFER_H_
#define _CHARACTERBUFFER_H_

#include "types.h"
#include "conddata/conddata.h"

// This class buffers exactly one character. The buffer size could be
// templated but in that case CircularBuffer should be used.
// The CharacterBuffer interface equals the CircularBuffer interface.
template <typename T>
class CharacterBuffer
{
public:
    CharacterBuffer ();
    bool put (T item) noexcept;
    CondData<T> get () noexcept;
    void reset () noexcept;
    bool empty () const noexcept;
    bool full () const noexcept;
    size_t capacity () const noexcept;
    size_t size () const noexcept;

private:
    T character_;
    bool filled_;
};

template <typename T>
CharacterBuffer<T>::CharacterBuffer ()
    :
    character_ {0},
    filled_ {0}
{}

template <typename T>
bool CharacterBuffer<T>::put (T item) noexcept
{
    if (filled_)
        return false;

    character_ = item;
    filled_ = true;
    return true;
}

template <typename T>
CondData<T> CharacterBuffer<T>::get () noexcept
{
    if (!filled_)
        return {};

    filled_ = false;
    return character_;
}

template <typename T>
void CharacterBuffer<T>::reset () noexcept
{
    filled_ = false;
}

template <typename T>
bool CharacterBuffer<T>::empty () const noexcept
{
    return !filled_;
}

template <typename T>
bool CharacterBuffer<T>::full () const noexcept
{
    return filled_;
}

template <typename T>
size_t CharacterBuffer<T>::capacity () const noexcept
{
    return 1;
}

template <typename T>
size_t CharacterBuffer<T>::size () const noexcept
{
    return (filled_) ? 1 : 0;
}

#endif // _CHARACTERBUFFER_H_