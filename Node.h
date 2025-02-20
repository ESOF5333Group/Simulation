#ifndef NODE_H
#define NODE_H

#include <queue>
#include "Source.h"
#include "Queue.h"

enum QueueType {
    PREMIUM,
    ASSURED,
    BEST_EFFORT
};

// Class to represent a network node
class Node {
public:
    void depart();
    void arrive(Packet packet);

    std::vector<Source> audioSources;
    std::vector<Source> videoSources;
    std::vector<Source> dataSources;
    int id;

    double nextOnTime();
    double nextOffTime();
    void switchNextSourceOn();
    void switchNextSourceOff();

    Queue* nextDepartQueue;
    double getNextDepartureTime() {
        nextDepartureTime = premiumQueue.getNextDepartureTime();
        nextDepartQueue = &premiumQueue;
        for (auto& queue: {&assuredQueue, &bestEffortQueue}) {
            if (queue->getNextDepartureTime() < nextDepartureTime) {
                nextDepartureTime = queue->getNextDepartureTime();
                nextDepartQueue = queue;
            }
        }
    }
private:
    static const int BUSY = 1;       // Mnemonics for server's being busy
    static const int IDLE = 0;       // and idle
    int status = IDLE;

    Queue premiumQueue;
    Queue assuredQueue;
    Queue bestEffortQueue;

    Packet packet;

    Source nextOnSource;
    Source nextOffSource;

    double nextDepartureTime;
    double nextArrivalTime;

    std::vector<std::vector<Source>*> sources = { &audioSources, &videoSources, &dataSources };
};;

#endif // NODE_H
