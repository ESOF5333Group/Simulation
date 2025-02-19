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
    // void arrive(const Packet& packet, QueueType queueType);
    // void processPackets();
	void depart();
    void arrive(Packet packet);

    std::vector<Source> audioSources;
    std::vector<Source> videoSources;
    std::vector<Source> dataSources;
    int id;

private:

    static const int BUSY = 1;       // Mnemonics for server's being busy
    static const int IDLE = 0;       // and idle
	int status = IDLE;

    Queue premiumQueue;
    Queue assuredQueue;
    Queue bestEffortQueue;

    Packet packet;
};

#endif // NODE_H
