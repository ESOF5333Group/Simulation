#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <queue>
#include "Source.h"

class Queue {
public:
    Queue(int capacity = 1e6) : capacity(capacity) { }

    int capacity;  // Limit on queue length

    void enqueue(Packet packet);

    Packet dequeue();

    bool isEmpty() const { return packets.empty(); }

    int getDroppedPackets() const { return droppedPackets; }

    int getSize() const { return static_cast<int>(packets.size()); }

private:
    std::queue<Packet> packets;

    double total_of_delays = 0;

    int droppedPackets = 0;

    int referenceDroppedPackets = 0;
};

#endif // QUEUE_H
