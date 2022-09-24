#ifndef _THREADSAFECONTAINER_H_
#define _THREADSAFECONTAINER_H_

#include <queue>
#include <mutex>
#include <condition_variable>

// The ThreadSafeContainer is the base class for ThreadSafeQueue and
// ThreadSafePriorityQueue. These classes differ in the use of the
// type of the queue and the definition of the member first(). (The
// used queues differ in their function members to get the "first" member
// in the queue.) The function first() is defined pure virtual as the
// derived classes have to provide their own implementation and to prevent
// instantiation of the base class.
template <typename T,
          template <typename Elem> class Cont>
class ThreadSafeContainer
{
    template <typename U>
    friend class ThreadSafeQueue;

    template <typename U>
    friend class ThreadSafePriorityQueue;

    Cont<T> queue_;
    mutable std::mutex guard;
    std::condition_variable signal;
    bool closed {false};

public:
    // bool fetchFirst(T &dest);
    bool tryPop (T &dest);
    bool waitAndPop(T& dest);

    bool push (T const &ir);
    size_t size();
    void close();

private:
    virtual T const &first() = 0;
};

// To prevent an exception on the return of a T object, the top element is
// moved to the destination and is then removed. This prevents the loss of
// queue elements.
template <typename T,
    template <typename Elem> class Cont>
bool ThreadSafeContainer<T, Cont>::tryPop(T &dest)
{
    std::lock_guard<std::mutex> lock(guard);

    if (queue_.empty() || closed)
        return false;

    // If this throws, nothing has been removed
    dest = std::move (first ());
    
    queue_.pop();
    return true;
}

template <typename T,
    template <typename Elem> class Cont>
bool ThreadSafeContainer<T, Cont>::waitAndPop(T& dest)
{
    std::unique_lock<std::mutex> lock(guard);

    if (closed)
        return false;

    while (queue_.empty())
        signal.wait(lock);

    // If this throws, nothing has been removed
    dest = std::move (first ());

    queue_.pop();
    return true;
}

template <typename T,
    template <typename Elem> class Cont>
bool ThreadSafeContainer<T, Cont>::push(T const& elem)
{
    // No elements can be pushed onto a closed queue
    if (closed)
        return false;

    {
        std::lock_guard<std::mutex> lock(guard);
        queue_.push(elem);
    }
    signal.notify_one();
    return true;
}

template <typename T,
          template <typename Elem> class Cont>
size_t ThreadSafeContainer<T, Cont>::size()
{
    std::lock_guard<std::mutex> lock(guard);
    return queue_.size();
}

template <typename T,
          template <typename Elem> class Cont>
void ThreadSafeContainer<T, Cont>::close()
{
    closed = true;
    signal.notify_all();
}
#endif // !_THREADSAFECONTAINER_H_
