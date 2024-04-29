#ifndef _THREADSAFEPRIOQUEUE_H_
#define _THREADSAFEPRIOQUEUE_H_

#include <set>
#include <mutex>
#include <condition_variable>
#include <exception>

using std::set;
using std::swap;
using std::move;
using std::mutex;
using std::unique_lock;
using std::lock_guard;
using std::condition_variable;
using std::logic_error;


// Implementation of a thread safe queue. The queue is implemented by
// means of a set because a set, contrary to a priority_queue,
// provides an iterator and an erase() function. Because of the use of
// set the queue can contain only unique elements. A set orders
// its elements in ascending order. The element with the highest priority
// therefore is at the end of the set.
template <typename T>
class ThreadSafePrioQueue
{
public:
    void clear();
    bool empty();
    bool contains (T const &elem);
    void erase (T const &elem);
    bool fetchTop(T &dest);
    void waitAndFetchTop(T &dest);
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

        ConstIterator (typename set<T>::const_iterator iter) : 
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
        typename set<T>::const_iterator ptr_;
    };

    ConstIterator cbegin() { return ConstIterator (queue_.cbegin()); }
    ConstIterator cend()   { return ConstIterator (queue_.cend()); }

private:
    set<T> queue_;
    mutable mutex guard;
    condition_variable signal;

    typename set<T>::iterator front () const; 
};

// Clear the contents of the priority queue
template <typename T>
void ThreadSafePrioQueue<T>::clear()
{
    set<T> emptyQueue;

    lock_guard<mutex> lock (guard);
    swap(queue_, emptyQueue);
}

template <typename T>
bool ThreadSafePrioQueue<T>::empty()
{
    lock_guard<mutex> lock (guard);
    return queue_.empty ();
}

// Check if the queue contains an element with the specified value
template <typename T>
bool ThreadSafePrioQueue<T>::contains (T const &elem)
{
    lock_guard<std::mutex> lock(guard);
    return queue_.contains (elem);
}

// Erase the element in the queue with the specified value
template <typename T>
 void ThreadSafePrioQueue<T>::erase (T const &elem)
{
    lock_guard<mutex> lock (guard);
    queue_.erase (elem);
}

// To prevent an exception on the return of a T object, the top element is
// moved to the destination and is then removed. This prevents the loss of
// queue elements.
template <typename T>
bool ThreadSafePrioQueue<T>::fetchTop(T &dest)
{
    lock_guard<mutex> lock (guard);

    if (queue_.empty ())
        return false;

    // If this throws, nothing has been removed
    dest = move (*front ());

    // Once we reach this point we own the queue's top element and
    // can safely remove it
    queue_.erase (front ());

    return true;
}

template <typename T>
void ThreadSafePrioQueue<T>::waitAndFetchTop(T &dest)
{
    unique_lock<mutex> lock (guard);

    signal.wait (lock, [this] () { return !queue_.empty ();});

    dest = move (*front ());
    queue_.erase (front ());
}


template <typename T>
void ThreadSafePrioQueue<T>::push (T const &elem)
{
    lock_guard<mutex> lock (guard);
    queue_.insert (elem);
    signal.notify_one ();
}

template <typename T>
size_t ThreadSafePrioQueue<T>::size()
{
    lock_guard<mutex> lock (guard);
    return queue_.size();
}

template <typename T>
T const &ThreadSafePrioQueue<T>::top() const
{
    lock_guard<mutex> lock (guard);

    if (queue_.empty ())
        throw logic_error ("top() on empty queue");

    return *front ();
}


// Private functions
// Return an iterator to the element at the front of the queue, i.e. the last
// element in the set. This function is private as it relies on the lock of
// the calling functions to keep the queue thread safe.
template <typename T>
typename set<T>::iterator ThreadSafePrioQueue<T>::front () const
{
    return --queue_.end ();
}

#endif // !_THREADSAFEPRIOQUEUE_H_
