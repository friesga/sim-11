#ifndef _INTERRUPTQUEUE_H_
#define _INTERRUPTQUEUE_H_

#include "../semaphore/semaphore.h"

#include <queue>

template <typename T>
class ThreadSafePrioQueue
{
    std::priority_queue<T> queue_;
    Semaphore semaphore_{1};        // One available priority_queue

public:
    void clear();
    bool empty() const;
    void pop();
    void push (T const &ir);
    size_t size() const;
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
void ThreadSafePrioQueue<T>::empty() const
{
    return queue_.empty();
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
size_t ThreadSafePrioQueue<T>::size() const
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
