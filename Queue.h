#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <queue>
#include "Source.h"

class Queue {
public:
    Queue(int capacity = 1e3) : capacity(capacity) { }

    int capacity;  // Limit on queue length

    void enqueue(Packet packet);

    Packet dequeue();

    bool isEmpty() const { return packets.empty(); }

    int getDroppedPackets() const { return droppedPackets; }
    void addDropPackets() { droppedPackets++; }
	void addRefDrop() { referenceDroppedPackets++; }

    int getSize() const { return static_cast<int>(packets.size()); }
private:
    std::queue<Packet> packets;

    int droppedPackets = 0;

    int referenceDroppedPackets = 0;
};

#endif // QUEUE_H
