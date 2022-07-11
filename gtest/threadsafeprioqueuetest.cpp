#include "threadsafeprioqueue/threadsafeprioqueue.h"

#include <thread>
#include <vector>
#include <queue>

#include <gtest/gtest.h>

using namespace std;

using PriorityQueue = ThreadSafePrioQueue<size_t>;

// Fill a regular queue in ascending order
void producer(PriorityQueue &queue, size_t index, size_t numReq)
{
    for (size_t num = 0; num < numReq; ++num)
    {
        queue.push(index * numReq + num);
    }
}

void fillQueue(PriorityQueue &queue, size_t nProducers, size_t nElements)
{
    vector<thread> producers;

    // Start n producers
    for (unsigned char index = 0; index < nProducers; ++index)
        producers.push_back (thread (producer, std::ref(queue), 
            index, nElements));
  
    // Wait till all producers have issued their requests
    for (size_t index = 0; index < nProducers; ++index)
        producers[index].join();
}


TEST (ThreadSafePrioQueue, PriorityQueueHandlesPushAndPop)
{
    size_t req;
    size_t const nProducers = 10;
    size_t const nRequestsPerProducer = 10;
    size_t numReq = nProducers * nRequestsPerProducer;
   
    PriorityQueue priorityQueue;

    // Fill the queue by several threads running at the same time
    fillQueue (priorityQueue, nProducers, nRequestsPerProducer);

    // Verify all elements are pushed
    ASSERT_EQ (priorityQueue.size(), numReq);

    // Verify all elements are retrieved in order of their priority
    size_t previousReq = numReq;
    while (numReq--)
    {
        priorityQueue.fetchTop (req);
        EXPECT_TRUE (req <= previousReq);
        previousReq = req;
    }

    // Verify all elements are retrieved
    EXPECT_EQ (priorityQueue.size(), 0);
}
