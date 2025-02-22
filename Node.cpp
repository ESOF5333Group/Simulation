#include "Node.h"
#include "globals.h"
#include "Queue.h"
#include <iostream>

void Node::arrive(Packet packet) {
    QueueType queueType{};
    switch (packet.type) {
        case AUDIO:
            queueType = PREMIUM;
            break;
        case VIDEO:
            queueType = ASSURED;
            break;
        case DATA:
            queueType = BEST_EFFORT;
            break;
    }
	Queue& queue = getQueue(queueType);
    packet.arrivalTime = sim_time;


    if(status == IDLE) {
        num_custs_delayed++;

        status = BUSY;

        servingPacket = packet;
        nextDepartureTime = sim_time + servingPacket.serviceTime;
	}
    else if (queue.getSize() < queue.capacity){
        queue.enqueue(packet);
    }
}

void Node::depart() {
    // Check if queue is empty
    //if (self.PremiumQueue.num_in_q != 0) {
    //    // Queue is empty, make server idle and eliminate departure event
    //    self.server_status = IDLE;
    //    self.time_next_event[2] = 1.0e+30;
    //}
    if (!servingPacket.isReference) {
        
    } else if (id < numNodes - 1) {
        referenceCounter++;
        // Forward packet to next node
        nodes[id + 1].arrive(servingPacket);
    } else {
        successfully_transmitted_packets++;
    }
    sumPacketDelay += sim_time - servingPacket.arrivalTime; // make calculation later
    numPacketTransmitted++;

    if (!premiumQueue.isEmpty()) {
        servingPacket = premiumQueue.dequeue();
        sumPacketDelay += sim_time - servingPacket.arrivalTime; // make calculation later

        num_custs_delayed++;
    
        nextDepartureTime = sim_time + servingPacket.serviceTime;
    } else if (!assuredQueue.isEmpty()) {
        servingPacket = assuredQueue.dequeue();
        sumPacketDelay += sim_time - servingPacket.arrivalTime; // make calculation later

        num_custs_delayed++;
    
        nextDepartureTime = sim_time + servingPacket.serviceTime;
    } else if (!bestEffortQueue.isEmpty()) {
        servingPacket = bestEffortQueue.dequeue();
        sumPacketDelay += sim_time - servingPacket.arrivalTime; // make calculation later

        num_custs_delayed++;
    
        nextDepartureTime = sim_time + servingPacket.serviceTime;
    }else if (bestEffortQueue.isEmpty()) {
        // All queues are empty
        status = IDLE;
        nextDepartureTime = std::numeric_limits<double>::max();
    }
}

double Node::nextOnTime() {
    double min_on_time = std::numeric_limits<double>::max();
    for (auto& sourcesOfType : sources) {
        for (auto& source : sourcesOfType) {
            double on_time = source.getNextOnTime();
            if (on_time < min_on_time) {
                min_on_time = on_time;
                next_on_source = &source;
            }
        }
    }
	return min_on_time;
}

double Node::nextOffTime() {
    double min_off_time = std::numeric_limits<double>::max();
    for (auto& sourcesOfType : sources) {
        for (auto& source : sourcesOfType) {
            double off_time = source.getNextOffTime();
            if (off_time < min_off_time) {
                min_off_time = off_time;
                next_off_source = &source;
            }
        }
    }
	return min_off_time;
}

double Node::getNextArrivalTime() {
    nextArrivalTime = std::numeric_limits<double>::max();
    for (auto typeSources : sources) {
        for (auto src : typeSources) {
            if (src.getStatus() == Source::Status::ON) {
                double arrivalTime = src.getnextPacketTime();
                if (arrivalTime < nextArrivalTime) {
                    nextArrivalTime = arrivalTime;
					next_packet_source = &src;
                }
            }
        }
    }
    return nextArrivalTime;
}

QueueType Node::getQueueType(PacketType packetType) {
    switch (packetType) {
    case AUDIO:
        return PREMIUM;
    case VIDEO:
        return ASSURED;
    case DATA:
        return BEST_EFFORT;
	default:
		throw std::runtime_error("Invalid PacketType");
    }
}

Queue& Node::getQueue(QueueType queueType) {
    switch (queueType) {
    case PREMIUM:
        return premiumQueue;
    case ASSURED:
        return assuredQueue;
    case BEST_EFFORT:
        return bestEffortQueue;
    }
    throw std::runtime_error("Invalid QueueType");
}