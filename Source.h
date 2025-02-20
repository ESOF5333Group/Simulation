#ifndef SOURCE_H
#define SOURCE_H

#include <vector>

// Structure to represent a packet
struct Packet {
    int id;          // Packet ID
    PacketType type;        // Packet size in bytes
	bool isReference; // Is the packet a reference packet
};

enum PacketType
{
    AUDIO,
	VIDEO,
	DATA
};

struct Config {
    int peakBitRate; // kbps
    double meanOnTime; // sec
    double meanOffTime; // sec
    int packetSize; // bytes
    int numSources; // number of sources
};

// Configuration for audio, video, and data sources
Config audioConfig = { 64, 0.36, 0.64, 120, 4 }; // kbps, sec, sec, bytes, number of sources
Config videoConfig = { 384, 0.33, 0.73, 1000, 4 }; // kbps, sec, sec, bytes, number of sources
Config dataConfig = { 256, 0.35, 0.65, 583, 4 }; // kbps, sec, sec, bytes, number of sources


class Source {
public:
    enum Status { OFF, ON };

    Source(int id, const Config& config, bool isReference);
	Packet nextPacket();

    double getNextOnTime() const {
        return nextOn;
    }
    double getNextOffTime() const {
        return nextOff;
    }

    void switchOn(double currentTime) {
        status = ON;
        std::exponential_distribution<> exp_dist(1.0 / meanOffTime);
        nextOff = currentTime + exp_dist(gen);
    }

    void switchOff(double currentTime) {
        status = OFF;
        std::exponential_distribution<> exp_dist(1.0 / meanOnTime);
        nextOn = currentTime + exp_dist(gen);
    }


private:
    int id;
    double meanOnTime;
    double meanOffTime;
	double nextOn;
	double nextOff;

    int packetSize;
    int peakBitRate;
    bool isReference;
	Status status;
    
    // Random number generation
    std::mt19937 gen;
    std::exponential_distribution<> exp_dist;
};

#endif // SOURCE_H

