//
// Copyright (c) Far Data Lab (FDL).
// All rights reserved.
//
//

#ifndef RINGBUF_H
#define RINGBUF_H


#include <atomic>
#include <iostream>
 
#define RING_SIZE           16777216
#define FORWARD_DEGREE      1048576
#define CACHE_LINE          64
#define INT_ALIGNED         16
 
template <class C>
using Atomic = std::atomic<C>;
typedef char*        BufferT;
typedef unsigned int MessageSizeT;
typedef unsigned int RingSizeT;
 
struct RingBuffer {
       Atomic<int> ForwardTail[INT_ALIGNED];
       Atomic<int> SafeTail[INT_ALIGNED];
       int Head[INT_ALIGNED];
       char Buffer[RING_SIZE];
};

RingBuffer* AllocateMessageBuffer(BufferT BufferAddress);

void DeallocateMessageBuffer(RingBuffer* Ring);

bool InsertToMessageBuffer(RingBuffer* Ring, const BufferT CopyFrom, MessageSizeT MessageSize);
 
bool FetchFromMessageBuffer(RingBuffer* Ring, BufferT CopyTo, MessageSizeT* MessageSize);
 
void ParseNextMessage(BufferT CurrentMessage, MessageSizeT CurrentMessageSize, BufferT* MessagePointer, MessageSizeT* MessageSize, BufferT* StartOfNext, MessageSizeT* RemainingSize);

#endif // RINGBUF_H