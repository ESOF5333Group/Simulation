#include "Source.h"
#include "globals.h"
#include <random>
#include <cmath>

static double generateExponentialRandom(double mean) {
    static std::mt19937 rng(std::random_device{}());
    std::exponential_distribution<double> dist(1.0 / mean);
    return dist(rng);
}

Source::Source(int id, Config config, bool isReference)
    : id(id), meanOnTime(config.meanOnTime), meanOffTime(config.meanOffTime), packetSize(config.packetSize), peakBitRate(config.peakBitRate), isReference(isReference) {
    nextOn = generateExponentialRandom(meanOnTime);
	nextOff = std::numeric_limits<double>::max();
	nextPacketTime = std::numeric_limits<double>::max();
	status = OFF;
	type = config.type;
	numGeneratedPackets = 0;
}

void Source::switchOn(double currentTime) {
    status = ON;
    std::exponential_distribution<> exp_dist(1.0 / meanOnTime);
    nextOff = currentTime + exp_dist(gen);
	nextOn = std::numeric_limits<double>::max();
    nextPacketTime = currentTime + packetSize * 8.0 / peakBitRate;
}

void Source::switchOff(double currentTime) {
    status = OFF;
    std::exponential_distribution<> exp_dist(1.0 / meanOffTime);
    nextOn = currentTime + exp_dist(gen);
	nextOff = std::numeric_limits<double>::max();
    nextPacketTime = std::numeric_limits<double>::max();
}

Packet Source::nextPacket()  
{  
   totalGenerated++; 
   numGeneratedPackets++;
   typeGenerated[type]++;
   nextPacketTime = simTime + packetSize * 8.0 / peakBitRate;
   if (isReference) refGenerated++;
   return Packet{type, isReference, simTime, static_cast<double>((packetSize * 8) / transmissionRate), simTime, packetSize};
}
