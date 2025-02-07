#include "System.h"
#include "Source.h"
#include <iostream>

System::System(int numNodes, PacketType refType, int numBackgroundAudioSources, int numBackgroundVideoSources, int numBackgroundDataSources, int bufferSize)
    : numBackgroundAudioSources(numBackgroundAudioSources) {
    for (int i = 0; i < numNodes; ++i) {
        nodes.emplace_back();
        // Create audio sources
        for (int j = 1; j <= numBackgroundAudioSources; ++j) {
            backgroundSources.emplace_back(j, audioConfig, false);
        }
        // Create video sources
        for (int j = 1; j <= numBackgroundVideoSources; ++j) {
            backgroundSources.emplace_back(j + numBackgroundAudioSources, videoConfig, false);
        }
        // Create data sources
        for (int j = 1; j <= numBackgroundDataSources; ++j) {
            backgroundSources.emplace_back(j + numBackgroundAudioSources + numBackgroundVideoSources, dataConfig, false);
        }
    }
}

void System::run() {
    // Generate and transmit background traffic
    for (auto& source : backgroundSources) {
        auto packets = source.generatePackets(numPackets);
        for (auto& packet : packets) {
            nodes[packet.id % nodes.size()].arrive(packet, BEST_EFFORT);
        }
    }

    // Generate and transmit reference traffic
    auto referencePackets = referenceSource.generatePackets(numPackets);
    for (auto& packet : referencePackets) {
        nodes[packet.id % nodes.size()].arrive(packet, PREMIUM);
    }

    // Process packets at each node
    for (auto& node : nodes) {
        node.processPackets();
    }
}

