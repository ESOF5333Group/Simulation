#ifndef SOURCE_H
#define SOURCE_H

#include <vector>
#include <random>
#include "config.h"

// Structure to represent a packet
struct Packet {
    PacketType type;        // Packet size in bytes
	bool isReference; // Is the packet a reference packet
    double arrivalTime; // Arrival time of the packet
    double serviceTime; // Service time of the packet
    double generatedTime;
    int size; // Packet size in bytes
};

class Source {
public:
    enum Status { OFF, ON };
    Source(int id = 0, Config config = audioConfig, bool isReference = false);
	int getId() const { return id; }
    PacketType getType() const { return type; }

    Packet nextPacket();

    Status getStatus() const { return status; }

    double getNextOnTime() const { return nextOn; }
    double getNextOffTime() const { return nextOff; }

    double getnextPacketTime() const { return nextPacketTime; }

    void switchOn(double currentTime);
    void switchOff(double currentTime);

	int getGeneratedPackets() const { return numGeneratedPackets; }

    Status status;

private:
    int id;
	PacketType type;
    int packetSize;
    int peakBitRate;
    bool isReference;
	
    double meanOnTime;
    double meanOffTime;
    double nextOn;
    double nextOff;
    double nextPacketTime;

	int numGeneratedPackets;
    
    // Random number generation
    std::mt19937 gen;
    std::exponential_distribution<> exp_dist;
};

#endif // SOURCE_H

