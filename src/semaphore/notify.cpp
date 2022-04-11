#include "semaphore.h"

void Semaphore::notify()
{
    // Get the lock
    std::lock_guard<std::mutex> lk(mutex_);    
    
    if (nAvailable_++ == 0)
        condition_.notify_one();
}
