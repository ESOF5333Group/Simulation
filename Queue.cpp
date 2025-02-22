#include "Queue.h"
#include <iostream>

double Queue::expon(double mean) {
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

static double generateR(double m) {
	double u = static_cast<double>(rand()) / RAND_MAX;

	double r = -log(1 - u) / m;

	return r;
}

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