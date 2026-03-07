#ifndef _THREADSAFEQUEUE_H_
#define _THREADSAFEQUEUE_H_

#include "threadsafecontainer.h"

// A thread safe queue is a thread safe container operating on
// a regular queue
template <typename T>
class ThreadSafeQueue : public ThreadSafeContainer<T, std::queue>
{
public:
    T const &first();
    bool empty () const;
};

// References to base class members is via the this pointer to make
// the references qualified name lookups
template <typename T>
T const &ThreadSafeQueue<T>::first()
{
    T const &elem = this->queue_.front();
    return elem;
}

template <typename T>
bool ThreadSafeQueue<T>::empty () const
{
    return this->queue_.empty ();
}

#endif // !_THREADSAFEQUEUE_H_