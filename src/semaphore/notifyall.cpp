#include "semaphore.h"

// Notify all threads waiting on this semaphore
void Semaphore::notify_all()
{
    // Get the lock
    std::lock_guard<std::mutex> lk(mutex_);    
    
    if (++nAvailable_ == 1)
        condition_.notify_all();
}
