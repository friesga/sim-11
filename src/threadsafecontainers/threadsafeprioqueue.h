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
// means of a set or multiset because sets, contrary to a priority_queue,
// provide an iterator and an erase() function. A set orders its elements
// in ascending order. The element with the highest priority is therefore
// at the end of the set.
// 
// The container to be used for the queue must be either a std::set or a
// std::multiset, the difference being that a std::set (and therefor the
// queue) will contain only unique elements while a std::multiset can contain
// duplicate elements.
//
// Iterating over the elements in the queue is not thread safe. The caller
// must take measures to safely use the iterator.
//
template <typename T, typename C = set<T>>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
class ThreadSafePrioQueue
{
public:
    using iterator = typename C::iterator;
    using const_iterator = typename C::const_iterator;

    void clear();
    bool empty();
    bool contains (T const &elem);
    bool erase (T const &elem);
    T const& top () const;
    bool fetchTop(T &dest);
    void waitAndFetchTop(T &dest);
    void push (T const &ir);
    size_t size();
    const_iterator cbegin () const;
    const_iterator cend () const;

private:
    C queue_;
    mutable mutex guard;
    condition_variable signal;

    typename C::iterator front () const; 
    const_iterator find (T const& elem) const;

};

// Clear the contents of the priority queue
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>) 
void ThreadSafePrioQueue<T, C>::clear()
{
    C emptyQueue;

    lock_guard<mutex> lock (guard);
    swap(queue_, emptyQueue);
}

template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
bool ThreadSafePrioQueue<T, C>::empty()
{
    lock_guard<mutex> lock (guard);
    return queue_.empty ();
}

// Check if the queue contains an element with the specified value
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
bool ThreadSafePrioQueue<T, C>::contains (T const &elem)
{
    lock_guard<std::mutex> lock(guard);
    return queue_.contains (elem);
}

// Erase the element with the specified value from the queue. The return value
// indicates whether the element was erased. 
//
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
 bool ThreadSafePrioQueue<T, C>::erase (T const &elem)
{
    lock_guard<mutex> lock (guard);
    return (queue_.erase (elem) > 0) ? true : false;
}

// To prevent an exception on the return of a T object, the top element is
// moved to the destination and is then removed. This prevents the loss of
// queue elements.
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
bool ThreadSafePrioQueue<T, C>::fetchTop(T &dest)
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

template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
void ThreadSafePrioQueue<T, C>::waitAndFetchTop(T &dest)
{
    unique_lock<mutex> lock (guard);

    signal.wait (lock, [this] () { return !queue_.empty ();});

    dest = move (*front ());
    queue_.erase (front ());
}


template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
void ThreadSafePrioQueue<T, C>::push (T const &elem)
{
    lock_guard<mutex> lock (guard);
    queue_.insert (elem);
    signal.notify_one ();
}

template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
size_t ThreadSafePrioQueue<T, C>::size()
{
    lock_guard<mutex> lock (guard);
    return queue_.size();
}

// Return a reference to the element with the highest priority. This function
// is not thread safe across calls to other functions in this class. The caller
// must ensure the queue is not empty when this function is called. The
// caller must also ensure that the reference returned is not used to 
// erase the element.
//
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
T const &ThreadSafePrioQueue<T, C>::top() const
{
    lock_guard<mutex> lock (guard);

    if (queue_.empty ())
        throw logic_error ("top() on empty queue");

    return *front ();
}

// Return a const_iterator to the beginning of the queue. The caller must ensure
// that the queue is not modified using the iterator returned by this function. 
//
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
C::const_iterator ThreadSafePrioQueue<T, C>::cbegin () const
{
    return queue_.cbegin ();
}

// This function returns an iterator to the element following the last element
// of the queue. This element acts as a placeholder; attempting to access it
// results in undefined behavior. This function is not thread safe across
// calls to other functions in this class.
//
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
C::const_iterator ThreadSafePrioQueue<T, C>::cend () const
{
    return queue_.cend ();
}

// Private functions
// Return an iterator to the element at the front of the queue, i.e. the last
// element in the set. This function is private as it relies on the lock of
// the calling functions to keep the queue thread safe.
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
typename C::iterator ThreadSafePrioQueue<T, C>::front () const
{
    return --queue_.end ();
}

// Return an iterator to the element with the specified value. If the element
// is not found, the iterator returned is equal to cend(). This function is
// private as it relies on the lock of the calling functions to keep the
// queue thread safe.
//
template <typename T, typename C>
    requires (std::same_as<C, set<T>> || std::same_as<C, std::multiset<T>>)
C::const_iterator ThreadSafePrioQueue<T, C>::find (T const& elem) const
{
    lock_guard<mutex> lock (guard);
    return queue_.find (elem);
}

#endif // !_THREADSAFEPRIOQUEUE_H_
