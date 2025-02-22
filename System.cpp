#include "System.h"
#include "Source.h"
#include "globals.h"
#include <iostream>

System::System(int numNodes, PacketType refType, int numBackgroundAudioSources, int numBackgroundVideoSources, int numBackgroundDataSources){
	referenceSource = Source(0, refType == PacketType::AUDIO ? audioConfig : refType == PacketType::VIDEO ? videoConfig : dataConfig, true);
    for (int i = 0; i < numNodes; ++i) {
        nodes.emplace_back();
        // Create audio sources
        for (int j = 1; j <= numBackgroundAudioSources; ++j) {
            nodes[i].audioSources.emplace_back(j, audioConfig, false);
        }
        // Create video sources
        for (int j = 1; j <= numBackgroundVideoSources; ++j) {
            nodes[i].videoSources.emplace_back(j, videoConfig, false);
        }
        // Create data sources
        for (int j = 1; j <= numBackgroundDataSources; ++j) {
            nodes[i].dataSources.emplace_back(j, dataConfig, false);
        }
    }
}

/*void System::run() {
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
}*/

