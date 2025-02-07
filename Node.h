#ifndef NODE_H
#define NODE_H

#include <queue>
#include "Source.h"

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

private:

    static const int BUSY = 1;       // Mnemonics for server's being busy
    static const int IDLE = 0;       // and idle
	int status = IDLE;

    std::queue<Packet> premiumQueue;
    std::queue<Packet> assuredQueue;
    std::queue<Packet> bestEffortQueue;

	std::vector<Source> audioSources;
	std::vector<Source> videoSources;
	std::vector<Source> dataSources;
};

#endif // NODE_H
