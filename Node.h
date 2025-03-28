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

    std::vector<Source> audioSources;
    std::vector<Source> videoSources;
    std::vector<Source> dataSources;
    int id;

    void depart();
    void arrive(Packet packet);
    void arriveBackground();

    double getNextOnTime();
    double getNextOffTime();

    double getNextDepartureTime() const { return nextDepartureTime; }
    double getNextArrivalTime();

    QueueType getQueueType(PacketType packetType);
    Queue& getQueue(QueueType queueType);

    double getSumPacketDelay() const { return sumPacketDelay; }
    double getQueueDelay(int queueIndex) const { return queueDelay[queueIndex]; }
	int getNumPacketTransmitted() const { return numPacketTransmitted; }
    int getQueueTransmitted(int queueIndex) const { return queueTransmitted[queueIndex]; }
	int getNumPacketArrive() const { return numPacketArrive; }

    std::vector<std::vector<Source>> getSources();

	Source* getSource(PacketType type, int index);

	void switchNextOn(double sim_time) { getSource(nextOnType, nextOnIndex)->switchOn(sim_time);}

	void switchNextOff(double sim_time) { getSource(nextOffType, nextOffIndex)->switchOff(sim_time);}

private:
    enum Status { BUSY, IDLE };
    Status status = IDLE;
    Queue premiumQueue = Queue(spqSize / 3);
    Queue assuredQueue = Queue(spqSize / 3);
    Queue bestEffortQueue = Queue(spqSize / 3);

    Packet servingPacket = {
        PacketType(), // Default enum value
        false,       // isReference
        0.0,        // arrivalTime
        0.0,        // serviceTime
        0           // size
    };

    double nextDepartureTime = std::numeric_limits<double>::max();
    double nextArrivalTime = std::numeric_limits<double>::max();

	double sumPacketDelay = 0.0;
    double queueDelay[3] = { 0, 0, 0 };
    int numPacketTransmitted = 0;
    int queueTransmitted[3] = { 0, 0, 0 };
	int numPacketArrive = 0;

	PacketType nextOnType = AUDIO;
	PacketType nextOffType = AUDIO;
    int nextOnIndex = 0;
	int nextOffIndex = 0;

	PacketType nextArriveType = AUDIO;
    int nextArriveIndex = 0;

    // int eachNode[3] = {};

    //std::vector<std::vector<Source>> getSources = { audioSources, videoSources, dataSources };
};

#endif // NODE_H
