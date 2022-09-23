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

    typename std::set<T>::iterator front () const;
 
public:
    void clear();
    bool empty();
    void erase (T const &elem);
    bool fetchTop(T &dest);
    void push (T const &ir);
    size_t size();
    T const &top() const;

    // Define a forward iterator over the elements in the queue. This is
    // in effect a constant iterator as objects in a set are const objects
    // to keep the ordering of the object in the set correct.
    struct ConstIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type const *;
        using reference         = value_type const &;

        ConstIterator (typename std::set<T>::const_iterator iter) : 
            ptr_ {iter} {};

        reference operator*() { return *ptr_; }
        pointer operator->()  { return &*ptr_; }

        // Prefix increment
        ConstIterator& operator++() { ptr_++; return *this; }  

        // Postfix increment
        ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const ConstIterator& a, const ConstIterator& b)
            { return a.ptr_ == b.ptr_; };
        friend bool operator!= (const ConstIterator& a, const ConstIterator& b)
            { return a.ptr_ != b.ptr_; };

    private:
        // Pointer to the element the iterator is pointing at
        // pointer ptr_;
        typename std::set<T>::const_iterator ptr_;
    };

    ConstIterator cbegin() { return ConstIterator (queue_.cbegin()); }
    ConstIterator cend()   { return ConstIterator (queue_.cend()); }
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
T const &ThreadSafePrioQueue<T>::top() const
{
    std::lock_guard<std::mutex> lock(guard);
    return *front();
}


// Private functions
// Return an iterator to the element at the front of the queue, i.e. the last
// element in the set. This function is private as it relies on the lock of
// the calling functions to keep the queue thread safe.
template <typename T>
typename std::set<T>::iterator ThreadSafePrioQueue<T>::front () const
{
    return --queue_.end();
}

#endif // !_THREADSAFEPRIOQUEUE_H_
