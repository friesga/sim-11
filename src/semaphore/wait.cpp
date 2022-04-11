#include "semaphore.h"

// Wait till semaphore is available
void Semaphore::wait()
{
    // Get the lock
    std::unique_lock<std::mutex> lk(mutex_);    

    // Release the lock and wait. On exit of wait() the lock
    // is acquired again.
    while (nAvailable_ == 0)
        condition_.wait(lk);           
        
    // Decrement available count. The lock is released in the
    // unique_lock destructor.
    --nAvailable_;
}