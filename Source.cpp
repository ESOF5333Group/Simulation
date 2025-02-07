#include "Source.h"
#include <random>
#include <cmath>

double generateExponentialRandom(double mean) {
    static std::mt19937 rng(std::random_device{}());
    std::exponential_distribution<double> dist(1.0 / mean);
    return dist(rng);
}

Source::Source(int id, const Config& config, bool isReference)
    : id(id), meanOnTime(config.meanOnTime), meanOffTime(config.meanOffTime), packetSize(config.packetSize), peakBitRate(config.peakBitRate), isReference(isReference) {
}


std::vector<Packet> Source::generatePackets(int numPackets) {
    std::vector<Packet> packets;
    double currentTime = 0.0;
    int packetId = 0;

    while (packetId < numPackets) {
        double onDuration = generateExponentialRandom(meanOnTime);
        double packetsDuringOn = onDuration * (peakBitRate * 1000) / (8.0 * packetSize);
        for (int i = 0; i < std::floor(packetsDuringOn) && packetId < numPackets; ++i) {
            packets.push_back({ packetId++, packetSize, currentTime, isReference });
            currentTime += (double)packetSize * 8 / (peakBitRate * 1000);
        }

        double offDuration = generateExponentialRandom(meanOffTime);
        currentTime += offDuration;
    }

    return packets;
}

