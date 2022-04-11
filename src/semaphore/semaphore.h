//JB: 1
//JB: But see my comment on locking during construction
#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

class Semaphore
{
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    size_t nAvailable_;

public:
    Semaphore(size_t nAvailable);
    void notify();                  // Notify one waiting thread 
    void notify_all();              // Notify all waiting threads
    size_t size() const;            // Return #available
    void wait();                    // Wait till semaphore is available
};

inline size_t Semaphore::size() const
{
    return nAvailable_;
}

#endif // !_SEMAPHORE_H_
