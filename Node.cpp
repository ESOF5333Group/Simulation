#include "Node.h"
#include <iostream>

void Node::arrive(const Packet& packet, QueueType queueType) {
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
    if (self.PremiumQueue.num_in_q != 0) {
        // Queue is not empty, process next customer
        --self.PremiumQueue.num_in_q;

        // Compute delay of customer who is beginning service
        double delay = self.sim_time - self.PremiumQueue.time_arrival[1];
        seif.total_of_delays += delay;

        // Increment number of customers delayed and schedule departure
        ++self.num_custs_delayed;  //condition 
        time_next_event[2] = sim_time + expon(mean_service);

        // Move each customer in queue up one position
        if (num_in_q > 0) {
            for (int i = 1; i <= num_in_q; ++i) {
                time_arrival[i] = time_arrival[i + 1];
            }
        } // If there are multiple queues this will need to be changed
    }
}
