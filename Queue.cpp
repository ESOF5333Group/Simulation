#include "Queue.h"
#include "globals.h"

void Queue::enqueue(Packet packet) {
     packets.push(packet);
}

Packet Queue::dequeue() {
    Packet packet = packets.front();
    packets.pop();
    return packet;
}