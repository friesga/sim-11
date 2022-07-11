#ifndef _THREADSAFEPRIOQUEUE_H_
#define _THREADSAFEPRIOQUEUE_H_

#include <set>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafePrioQueue
{
    std::set<T> queue_;
    mutable std::mutex guard;
    std::condition_variable signal;
 
public:
    void clear();
    bool empty();
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
    dest = std::move (*(--queue_.end()));

    // Once we reach this point we own the queue's top element and
    // can safely remove it
    queue_.erase(--queue_.end());

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
    return *(--queue_.end());
}

#endif // !_THREADSAFEPRIOQUEUE_H_
