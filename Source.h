#ifndef SOURCE_H
#define SOURCE_H

#include <vector>
#include <random>
#include <queue>

// Structure to represent a packet
struct Packet {
    int id;              // Packet ID
    PacketType type;     // Packet type (audio, video, data)
    bool isReference;    // Is the packet a reference packet
    double arrivalTime;  // Time when the packet is generated
};

enum PacketType {
    AUDIO,
    VIDEO,
    DATA
};

struct Config {
    int peakBitRate;     // kbps
    double meanOnTime;   // sec
    double meanOffTime;  // sec
    int packetSize;      // bytes
    int numSources;      // number of sources
};

// Configuration for audio, video, and data sources
Config audioConfig = { 64, 0.36, 0.64, 120, 4 }; // kbps, sec, sec, bytes, number of sources
Config videoConfig = { 384, 0.33, 0.73, 1000, 4 }; // kbps, sec, sec, bytes, number of sources
Config dataConfig = { 256, 0.35, 0.65, 583, 4 }; // kbps, sec, sec, bytes, number of sources

class Source {
public:
    enum Status { OFF, ON };

    Source(int id, const Config& config, bool isReference)
        : id(id), meanOnTime(config.meanOnTime), meanOffTime(config.meanOffTime),
        packetSize(config.packetSize), peakBitRate(config.peakBitRate),
        isReference(isReference), status(OFF), nextPacketTime(0.0) {
        // Initialize random number generator
        std::random_device rd;
        gen = std::mt19937(rd());

        // Schedule the first state transition
        scheduleNextStateTransition(0.0);
    }

    // Generate the next packet (if available)
    bool nextPacket(double currentTime, Packet& packet) {
        if (status == ON && currentTime >= nextPacketTime) {
            // Create a new packet
            packet.id = id;
            packet.type = (peakBitRate == 64) ? AUDIO : (peakBitRate == 384) ? VIDEO : DATA;
            packet.isReference = isReference;
            packet.arrivalTime = currentTime;

            // Schedule the next packet generation
            double interPacketTime = (packetSize * 8.0) / (peakBitRate * 1000.0); // in seconds
            nextPacketTime = currentTime + interPacketTime;

            return true; // Packet generated
        }
        return false; // No packet generated
    }

    // Get the time of the next event (state transition or packet generation)
    double getNextEventTime() const {
        if (status == ON) {
            return std::min(nextStateTransitionTime, nextPacketTime);
        }
        else {
            return nextStateTransitionTime;
        }
    }

    // Handle state transitions
    void updateState(double currentTime) {
        if (currentTime >= nextStateTransitionTime) {
            if (status == ON) {
                switchOff(currentTime);
            }
            else {
                switchOn(currentTime);
            }
        }
    }

private:
    int id;
    double meanOnTime;
    double meanOffTime;
    int packetSize;
    int peakBitRate;
    bool isReference;
    Status status;

    double nextStateTransitionTime; // Time of the next state transition
    double nextPacketTime;          // Time of the next packet generation

    // Random number generation
    std::mt19937 gen;
    std::exponential_distribution<> expDist;

    // Schedule the next state transition
    void scheduleNextStateTransition(double currentTime) {
        if (status == ON) {
            // Transition to OFF state
            double offDuration = expDist(gen) * meanOffTime;
            nextStateTransitionTime = currentTime + offDuration;
            status = OFF;
        }
        else {
            // Transition to ON state
            double onDuration = expDist(gen) * meanOnTime;    //check the equation
            nextStateTransitionTime = currentTime + onDuration;
            status = ON;
            // Schedule the first packet generation
            nextPacketTime = currentTime;
        }
    }

    // Switch to ON state
    void switchOn(double currentTime) {
        status = ON;
        double onDuration = expDist(gen) * meanOnTime;
        nextStateTransitionTime = currentTime + onDuration;
        nextPacketTime = currentTime; // Schedule the first packet generation
    }

    // Switch to OFF state
    void switchOff(double currentTime) {
        status = OFF;
        double offDuration = expDist(gen) * meanOffTime;
        nextStateTransitionTime = currentTime + offDuration;
    }
};

#endif // SOURCE_H