#include "ringbuf_test.h"
#include <iostream>


void ProducerQ1(RingBuffer* ring) {
    char message[MESSAGE_SIZE];
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        *(int64_t*)message = i;
        while (!InsertToMessageBuffer(ring, message, MESSAGE_SIZE)) {
            std::this_thread::yield();
        }
    }
}


void ConsumerQ1(RingBuffer* ring) {
    int64_t numMessagesReceived = 0;
    char *buffer = new char[FORWARD_DEGREE];
    memset(buffer, 0, FORWARD_DEGREE);
    while (numMessagesReceived < NUM_MESSAGES) {
        MessageSizeT receivedSize = 0;
        if (FetchFromMessageBuffer(ring, buffer, &receivedSize)) {
            char* currentMessage = (char*) buffer;
            MessageSizeT remainingSize = receivedSize;
            while (remainingSize > 0) {
                BufferT messagePointer;
                MessageSizeT messageSize;
                BufferT startOfNext;
                ParseNextMessage(currentMessage, remainingSize, &messagePointer, &messageSize, &startOfNext, &remainingSize);
                if(*(int64_t*)messagePointer != numMessagesReceived) {
                    std::cout << "Expected " << numMessagesReceived << " but got " << *(int64_t*)messagePointer << std::endl;
                    return;
                }
                numMessagesReceived++;
                currentMessage = startOfNext;
            }
        } else {
            std::this_thread::yield(); 
        }
        memset(buffer, 0, FORWARD_DEGREE);
    }
}


void ProducerQ2(RingBuffer* ring, uint32_t id, uint32_t numProducers) {
    char message[MESSAGE_SIZE];
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        *(int64_t*)message = i*numProducers + id;
        while (!InsertToMessageBufferQ2(ring, message, MESSAGE_SIZE)) {
            std::this_thread::yield();
        }
    }
}


void ConsumerQ2(RingBuffer* ring, uint32_t numProducers) {
    int64_t numMessagesReceived = 0;
    char *buffer = new char[FORWARD_DEGREE];
    memset(buffer, 0, FORWARD_DEGREE);
    std::vector<uint64_t> receivedMessages(numProducers, 0);
    for(int i = 0; i < numProducers; ++i) {
        receivedMessages[i] = i;
    }

    while (numMessagesReceived < NUM_MESSAGES*numProducers) {
        MessageSizeT receivedSize = 0;
        if (FetchFromMessageBuffer(ring, buffer, &receivedSize)) {
            char* currentMessage = (char*) buffer;
            MessageSizeT remainingSize = receivedSize;
            while (remainingSize > 0) {
                BufferT messagePointer;
                MessageSizeT messageSize;
                BufferT startOfNext;
                ParseNextMessage(currentMessage, remainingSize, &messagePointer, &messageSize, &startOfNext, &remainingSize);
                uint32_t producerId = *(int64_t*)messagePointer % numProducers;
                if(*(int64_t*)messagePointer != receivedMessages[producerId]) {
                    std::cout << "Expected " << receivedMessages[producerId] << " but got " << *(int64_t*)messagePointer << " " << remainingSize << " " << receivedSize <<std::endl;
                    return;
                }
                receivedMessages[producerId] += numProducers;
                numMessagesReceived++;
                currentMessage = startOfNext;
            }
        } else {
            std::this_thread::yield(); 
        }
        memset(buffer, 0, FORWARD_DEGREE);
    }
}


void testQ1() {
    char *buffer = new char[sizeof(RingBuffer)];
    RingBuffer* ring = AllocateMessageBuffer(buffer);
    std::thread producerThread(ProducerQ1, ring);
    auto startTime = std::chrono::high_resolution_clock::now();
    std::thread consumerThread(ConsumerQ1, ring);
    producerThread.join();
    consumerThread.join();
    auto endTime = std::chrono::high_resolution_clock::now();
    DeallocateMessageBuffer(ring);
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms" << std::endl;
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "Throughput: "
            << NUM_MESSAGES / diff.count()
            << " messages/s" << std::endl;
}


void testQ2(uint32_t numProducers) {
    char *buffer = new char[sizeof(RingBuffer)];
    RingBuffer* ring = AllocateMessageBuffer(buffer);
    std::vector<std::thread> producerThreads(numProducers);
    for(int i = 0; i < numProducers; ++i) {
        producerThreads[i] = std::thread(ProducerQ2, ring, i, numProducers);
    }
    auto startTime = std::chrono::high_resolution_clock::now();
    std::thread consumerThread(ConsumerQ2, ring, numProducers);
    for(int i = 0; i < numProducers; ++i) {
        producerThreads[i].join();
    }
    consumerThread.join();
    auto endTime = std::chrono::high_resolution_clock::now();
    DeallocateMessageBuffer(ring);
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms" << std::endl;
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "Throughput: "
            << NUM_MESSAGES*numProducers / diff.count()
            << " messages/s" << std::endl;
}