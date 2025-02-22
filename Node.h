#ifndef NODE_H
#define NODE_H

#include "Queue.h"
#include "Source.h"

enum QueueType {
    PREMIUM,
    ASSURED,
    BEST_EFFORT
};

// Class to represent a network node
class Node {
public:
	Node(int id) : id(id) {};
    void depart();
    void arrive(Packet packet);

    std::vector<Source> audioSources;
    std::vector<Source> videoSources;
    std::vector<Source> dataSources;
    int id;

    double nextOnTime();
    double nextOffTime();

    double getNextDepartureTime() const {
        return nextDepartureTime;
    }

    double getNextArrivalTime();

    QueueType getQueueType(PacketType packetType);

    Queue& getQueue(QueueType queueType);

	double getSumPacketDelay() const {
		return sumPacketDelay;
	}

	double getNumPacketTransmitted() const {
		return numPacketTransmitted;
	}

private:
    enum Status { BUSY, IDLE };
    Status status = IDLE;
    Queue premiumQueue;
    Queue assuredQueue;
    Queue bestEffortQueue;

    Packet servingPacket;

    Source nextOnSource;
    Source nextOffSource;

    double nextDepartureTime = std::numeric_limits<double>::max();
    double nextArrivalTime = std::numeric_limits<double>::max();

	double sumPacketDelay = 0.0;
    int numPacketTransmitted = 0;

    std::vector<std::vector<Source>> sources = { audioSources, videoSources, dataSources };
};

#endif // NODE_H
