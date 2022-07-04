#ifndef _INTERRUPTQUEUE_H_
#define _INTERRUPTQUEUE_H_

#include "semaphore/semaphore.h"

#include <queue>

template <typename T>
class ThreadSafePrioQueue
{
    std::priority_queue<T> queue_;
    Semaphore semaphore_{1};        // One available priority_queue

public:
    void clear();
    bool empty();
    bool fetchTop(T &dest);
    void pop();
    void push (T const &ir);
    size_t size();
    T const &top();
};

// Clear the contents of the priority queue
template <typename T>
void ThreadSafePrioQueue<T>::clear()
{
    std::priority_queue<T> emptyQueue;
    std::swap(queue_, emptyQueue);
}

template <typename T>
bool ThreadSafePrioQueue<T>::empty()
{
    return queue_.empty();
}

// To prevent an exception on the return of a T object, the top element is
// moved to the destination and is then removed. This prevents the loss of
// queue elements.
template <typename T>
bool ThreadSafePrioQueue<T>::fetchTop(T &dest)
{
    semaphore_.wait();

    if (queue_.empty())
        return false;

    // If this throws, nothing has been removed
    dest = std::move (queue_.top());

    // Once we reach this point we own the queue's top element and
    // can safely remove it
    queue_.pop();

    semaphore_.notify();
    return true;
}

template <typename T>
void ThreadSafePrioQueue<T>::pop()
{
    semaphore_.wait();
    queue_.pop();
    semaphore_.notify();
}

template <typename T>
void ThreadSafePrioQueue<T>::push (T const &elem)
{
    semaphore_.wait();
    queue_.push (elem);
    semaphore_.notify();
}

template <typename T>
size_t ThreadSafePrioQueue<T>::size()
{
    semaphore_.wait();
    size_t size = queue_.size();
    semaphore_.notify();

    return size;
}

template <typename T>
T const &ThreadSafePrioQueue<T>::top()
{
    semaphore_.wait();
    T const &elem = queue_.top();
    semaphore_.notify();

    return elem;
}

#endif // !_INTERRUPTQUEUE_H_
