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
    Source(int id, const Config& config, bool isReference);
	Packet nextPacket();

private:
    int id;
    double meanOnTime;
    double meanOffTime;
	double nextOn;
	double nextOff;

    int packetSize;
    int peakBitRate;
    bool isReference;
	bool state = 0; // 0 = off, 1 = on
    
    // Random number generation
    std::mt19937 gen;
    std::exponential_distribution<> exp_dist;
};

#endif // SOURCE_H

