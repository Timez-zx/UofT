#ifndef RINGBUF_TEST_H_
#define RINGBUF_TEST_H_

#include "ringbuf.h"
#include <sys/types.h>
#include <thread>
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>

const int NUM_MESSAGES = 100000000;
const int MESSAGE_SIZE = 8;

void ProducerQ1(RingBuffer* ring);

void ConsumerQ1(RingBuffer* ring);

void testQ1();

void ProducerQ2(RingBuffer* ring, uint32_t id, uint32_t numProducers);

void ConsumerQ2(RingBuffer* ring, uint32_t numProducers);

void testQ2(uint32_t numProducers);

#endif // RINGBUF_TEST_H_
