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
    nextOn = generateExponentialRandom(meanOnTime);
}



