#include "semaphore.h"

Semaphore::Semaphore(size_t nAvailable)
{
    // Get a lock on the mutex and initialize the available count
    std::unique_lock<std::mutex> lk(mutex_);

    nAvailable_ = nAvailable;
}

