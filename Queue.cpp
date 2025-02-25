#include "Queue.h"

void Queue::enqueue(Packet packet) {
    if (static_cast<int>(packets.size()) < capacity) {
        packets.push(packet);

    }
    else {
        droppedPackets++;
        if (packet.isReference) {
            referenceDroppedPackets++;
        }
    }
}

Packet Queue::dequeue() {
    if (!packets.empty()) {
        Packet packet = packets.front();
        packets.pop();

        return packet;
    }
    throw std::runtime_error("Queue is empty");
}