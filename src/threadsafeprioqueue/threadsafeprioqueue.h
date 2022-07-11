#ifndef _THREADSAFEPRIOQUEUE_H_
#define _THREADSAFEPRIOQUEUE_H_

#include <set>
#include <mutex>
#include <condition_variable>

// Implementation of a thread safe queue. The queue is implemented by
// means of a std::set because a set, contrary to a std::priority_queue,
// provides an iterator and an erase() function. Because of the use of
// std::set the queue can contain only unique elements. A std::set orders
// its elements in ascending order. The element with the highest priority
// therefore is at the end of the set.
template <typename T>
class ThreadSafePrioQueue
{
    std::set<T> queue_;
    mutable std::mutex guard;
    std::condition_variable signal;

    typename std::set<T>::iterator front ();
 
public:
    void clear();
    bool empty();
    void erase (T const &elem);
    bool fetchTop(T &dest);
    void push (T const &ir);
    size_t size();
    T const &top();
};

// Clear the contents of the priority queue
template <typename T>
void ThreadSafePrioQueue<T>::clear()
{
    std::set<T> emptyQueue;

    std::lock_guard<std::mutex> lock(guard);
    std::swap(queue_, emptyQueue);
}

template <typename T>
bool ThreadSafePrioQueue<T>::empty()
{
    std::lock_guard<std::mutex> lock(guard);
    return queue_.empty();
}

// Erase the element in the queue with the specified value
template <typename T>
void ThreadSafePrioQueue<T>::erase (T const &elem)
{
    std::lock_guard<std::mutex> lock(guard);
    queue_.erase (elem);
}

// To prevent an exception on the return of a T object, the top element is
// moved to the destination and is then removed. This prevents the loss of
// queue elements.
template <typename T>
bool ThreadSafePrioQueue<T>::fetchTop(T &dest)
{
    std::unique_lock<std::mutex> lock(guard);

    if (queue_.empty())
        signal.wait(lock);

    // If this throws, nothing has been removed
    dest = std::move (*front());

    // Once we reach this point we own the queue's top element and
    // can safely remove it
    queue_.erase(front());

    return true;
}


template <typename T>
void ThreadSafePrioQueue<T>::push (T const &elem)
{
    std::lock_guard<std::mutex> lock(guard);
    queue_.insert (elem);
}

template <typename T>
size_t ThreadSafePrioQueue<T>::size()
{
    std::lock_guard<std::mutex> lock(guard);
    return queue_.size();
}

template <typename T>
T const &ThreadSafePrioQueue<T>::top()
{
    std::lock_guard<std::mutex> lock(guard);
    return *front();
}


// Private functions
// Return an iterator to the element at the front of the queue, i.e. the last
// element in the set. This function is private as it relies on the lock of
// the calling functions to keep the queue thread safe.
template <typename T>
typename std::set<T>::iterator ThreadSafePrioQueue<T>::front ()
{
    return --queue_.end();
}

#endif // !_THREADSAFEPRIOQUEUE_H_