#include "threadsafecontainers/threadsafeprioqueue.h"

#include <thread>
#include <vector>

#include <gtest/gtest.h>

using std::thread;
using std::vector;


using PriorityQueue = ThreadSafePrioQueue<int>;

// Fill a regular queue in ascending order
void producer (PriorityQueue &queue, int index, int numReq)
{
    for (int num = 0; num < numReq; ++num)
    {
        queue.push(index * numReq + num);
    }
}

// Erase all odd elements in the queue
void eraser (PriorityQueue &queue, int numReq)
{
    for (int num = 0; num < numReq; ++num)
    {
        if (num % 2 == 1)
            queue.erase (num);
    }
}

void fillQueue (PriorityQueue &queue, int nProducers, int nElements)
{
    vector<thread> producers;

    // Start n producers
    for (unsigned char index = 0; index < nProducers; ++index)
        producers.push_back (thread (producer, std::ref(queue), 
            index, nElements));
  
    // Wait till all producers have issued their requests
    for (int index = 0; index < nProducers; ++index)
        producers[index].join();
}


TEST (ThreadSafePrioQueue, isThreadSafe)
{
    int req;
    int const nProducers = 10;
    int const nRequestsPerProducer = 10;
    int numReq = nProducers * nRequestsPerProducer;
   
    PriorityQueue priorityQueue;

    // Fill the queue by several threads running at the same time
    fillQueue (priorityQueue, nProducers, nRequestsPerProducer);

    // Verify all elements are pushed
    ASSERT_EQ (priorityQueue.size(), numReq);

    // Verify all elements are retrieved in order of their priority
    int previousReq = numReq;
    while (numReq--)
    {
        priorityQueue.fetchTop (req);
        EXPECT_TRUE (req <= previousReq);
        previousReq = req;
    }

    // Verify all elements are retrieved
    EXPECT_EQ (priorityQueue.size(), 0);
}

TEST (ThreadSafePrioQueue, isErasable)
{
    int req;
    int numReq = 100;
   
    PriorityQueue priorityQueue;

    // Fill the queue with the values 0- 99
    producer (priorityQueue, 0, numReq);

    // Verify all elements are pushed
    ASSERT_EQ (priorityQueue.size(), numReq);

    // Erase all odd elements
    eraser (priorityQueue, numReq);

    // Verify all elements in the queue are even
    while (priorityQueue.size() > 0)
    {
        priorityQueue.fetchTop (req);
        EXPECT_TRUE (req % 2 == 0);
    }
}

// Verify that empty() on an empty queue return false and that fetching
// from an empty queue returns false.
TEST (ThreadSafePrioQueue, emptyQueueIsDetected)
{
    PriorityQueue priorityQueue;

    EXPECT_TRUE (priorityQueue.empty ());

    int dummy;
    EXPECT_FALSE (priorityQueue.fetchTop (dummy));

    // Now push an element on the queue to verify it's not empty anymore
    priorityQueue.push (42);
    EXPECT_FALSE (priorityQueue.empty ());

    // Verify this element can be read and the queue is empty again
    EXPECT_TRUE (priorityQueue.fetchTop (dummy));
    EXPECT_EQ (dummy, 42);
    EXPECT_TRUE (priorityQueue.empty ());
}

TEST (ThreadSafePrioQueue, topOnEmptyQueueThrows)
{
    PriorityQueue priorityQueue;

    try
	{
		int dummy = priorityQueue.top ();
		FAIL();
	}
	catch (std::logic_error const &except)
	{
		EXPECT_STREQ (except.what(), "top() on empty queue");
	}
	catch (...)
	{
		FAIL();
	}
}

void sender (ThreadSafePrioQueue<int>* messageQueue,
    ThreadSafePrioQueue<int>* ackQueue)
{
    for (int message = 0; message < 9; ++message)
    {
        int answer;
        messageQueue->push (message);
        ackQueue->waitAndFetchTop (answer);

        EXPECT_EQ (answer, message);
    }
}

void receiver (ThreadSafePrioQueue<int>* messageQueue,
    ThreadSafePrioQueue<int>* ackQueue)
{
    for (int i = 0; i < 9; ++i)
    {
        int receivedMessage;
        messageQueue->waitAndFetchTop (receivedMessage);

        // Verify the sender hasn't pushed the next message yet
        // so we actually have been waiting.
        EXPECT_TRUE (messageQueue->empty ());

        ackQueue->push (receivedMessage);
    }
}

TEST (ThreadSafePrioQueue, waitOnQueueSucceeds)
{
    ThreadSafePrioQueue<int> message;
    ThreadSafePrioQueue<int> ack;
    thread t1 {sender, &message, &ack};
    thread t2 {receiver, &message, &ack};

    t1.join ();
    t2.join ();
}

TEST (ThreadSafePrioQueue, identicalElementsCanBePushed)
{
    ThreadSafePrioQueue<int, std::multiset<int>> priorityQueue;

    // Now push an element on the queue to verify it's not empty anymore
    priorityQueue.push (42);
    priorityQueue.push (42);
    EXPECT_EQ (priorityQueue.size (), 2);
}