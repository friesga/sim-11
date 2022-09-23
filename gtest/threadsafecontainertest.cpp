#include "threadsafecontainers/threadsafequeue.h"

#include <thread>
#include <vector>
#include <queue>

#include <gtest/gtest.h>

using namespace std;

using RegularQueue = ThreadSafeQueue<size_t>;

// Fill a regular queue in ascending order
template <typename T>
void producer(T &queue, size_t numReq)
{
    for (size_t num = 0; num < numReq; ++num)
    {
        queue.push(num);
    }
}

template <typename T>
void fillQueue(T &queue, size_t nProducers, size_t nElements)
{
    vector<thread> producers;

    // Start n producers
    for (unsigned char index = 0; index < nProducers; ++index)
        producers.push_back (thread (producer<T>, std::ref(queue), 
            nElements));
  
    // Wait till all producers have issued their requests
    for (size_t index = 0; index < nProducers; ++index)
        producers[index].join();
}


TEST (ThreadSafeContainers, RegularQueueHandlesPushAndPop)
{
    size_t req;
    size_t const nProducers = 10;
    size_t const nRequestsPerProducer = 10;
    size_t numReq = nProducers * nRequestsPerProducer;
   
    RegularQueue regularQueue;

    fillQueue<RegularQueue> (regularQueue, nProducers, nRequestsPerProducer);

    // Verify all elements are pushed
    ASSERT_EQ (regularQueue.size(), numReq);

    while (numReq--)
    {
        regularQueue.waitAndPop (req);
    }

    EXPECT_EQ (regularQueue.size(), 0);
}
