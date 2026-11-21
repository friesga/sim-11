#ifndef _GUARDED_H_
#define _GUARDED_H_

#include <mutex>

// Guarded wraps an object that is shared between threads and serializes
// all access to it by means of a mutex. The object can only be accessed
// via a Proxy which holds the mutex locked during its lifetime.
//
// - Access for the duration of a single expression:
//      guarded->field = 1;
// - Access for the duration of a block of statements:
//      {
//          auto proxy = guarded.lock ();
//          proxy->field = !proxy->field;
//      }
// - Copy of the complete object:
//      T copy = guarded.load ();
//
// The mutex is not recursive. Using the Guarded object twice in a single
// expression (e.g. 'guarded->a = guarded->b;') or within a lock() block
// deadlocks.
//
template <typename T>
class Guarded
{
public:
    class Proxy
    {
    public:
        Proxy (T& object, std::mutex& mutex) : lock_ {mutex}, object_ {object} {}

        T* operator-> () { return &object_; }
        T& operator* () { return object_; }

    private:
        std::unique_lock<std::mutex> lock_;
        T& object_;
    };

    Guarded () = default;
    explicit Guarded (T const& object) : object_ {object} {}

    Guarded (Guarded const&) = delete;
    Guarded& operator= (Guarded const&) = delete;

    Proxy operator-> () { return Proxy {object_, mutex_}; }
    Proxy lock () { return Proxy {object_, mutex_}; }

    T load () const
    {
        std::lock_guard<std::mutex> lock {mutex_};
        return object_;
    }

    void store (T const& object)
    {
        std::lock_guard<std::mutex> lock {mutex_};
        object_ = object;
    }

private:
    T object_ {};
    mutable std::mutex mutex_;
};

#endif // _GUARDED_H_