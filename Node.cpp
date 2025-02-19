#include "Node.h"
#include <iostream>

void Node::arrive(Packet packet) {
    if(status == IDLE) {
        
    } else if(packet.type == AUDIO) {
        if (premiumQueue.num_in_q == k) {
            status = BUSY;
            time_next_event[2] = sim_time + expon(mean_service);
        } else {
            ++premiumQueue.num_in_q;
            premiumQueue.time_arrival[premiumQueue.num_in_q] = sim_time;
        }
    } else if(packet.type == VIDEO) {
        videoSources.emplace_back(id, videoConfig, packet.isReference);
    } else if(packet.type == DATA) {
        dataSources.emplace_back(id, dataConfig, packet.isReference);

    }
    switch (queueType) {
    case PREMIUM:
        premiumQueue.push(packet);
        break;
    case ASSURED:
        assuredQueue.push(packet);
        break;
    case BEST_EFFORT:
        bestEffortQueue.push(packet);
        break;
    }
    std::cout << "Packet ID: " << packet.id << " received at node in queue " << queueType << ".\n";
}

void Node::processPackets() {
    // Process premium queue first
    while (!premiumQueue.empty()) {
        Packet packet = premiumQueue.front();
        premiumQueue.pop();
        std::cout << "Processing Packet ID: " << packet.id << " from premium queue at node.\n";
    }

    // Process assured queue next
    while (!assuredQueue.empty()) {
        Packet packet = assuredQueue.front();
        assuredQueue.pop();
        std::cout << "Processing Packet ID: " << packet.id << " from assured queue at node.\n";
    }

    // Process best-effort queue last
    while (!bestEffortQueue.empty()) {
        Packet packet = bestEffortQueue.front();
        bestEffortQueue.pop();
        std::cout << "Processing Packet ID: " << packet.id << " from best-effort queue at node.\n";
    }
}



void Node::depart() {
    // Check if queue is empty
    //if (self.PremiumQueue.num_in_q != 0) {
    //    // Queue is empty, make server idle and eliminate departure event
    //    self.server_status = IDLE;
    //    self.time_next_event[2] = 1.0e+30;
    //}
    if (premiumQueue.num_in_q != 0) {
        premiumQueue.depart();
    } else if (assuredQueue.num_in_q != 0) {
        assuredQueue.depart();
    } else if (bestEffortQueue.num_in_q != 0) {
        bestEffortQueue.depart();
    } else if (bestEffortQueue.num_in_q = 0) {
        // All queues are empty
        status = IDLE;
    }

    if (!packet.isReference) {
        discard(packet);
    } else if (id < M) {
        // Forward packet to next node
        nodes[id + 1].arrive(packet, PREMIUM);
    } else {
        recieved++;
    }
}
