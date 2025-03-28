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
    packet.arrivalTime = simTime;
    numPacketArrive++;
    queue.addArrive();

    if(status == IDLE) {
        numPackets++;
        status = BUSY;
        servingPacket = packet;
        nextDepartureTime = simTime + servingPacket.serviceTime;
	}
    else if (queue.getSize() < queue.capacity){
        queue.enqueue(packet);
        queue.addBacklog();
        typeBacklogged[packet.type]++;
    }
    else {
        queue.addDropPackets();
        dropped++;
        typeDropped[packet.type]++;
        if (packet.isReference) {
            queue.addRefDrop();
            refDropped++;
        }
    }
}

void Node::arriveBackground() {
    Packet packet = getSource(nextArriveType, nextArriveIndex)->nextPacket();
    arrive(packet);
}

void Node::depart() {
    if (servingPacket.isReference && id < numNodes - 1) {
        // Ref forwarded to next node
        refDeparts++;       
        nodes[id + 1].arrive(servingPacket);
    } else if (servingPacket.isReference) {
        // Ref reach destination
        refSumDelay += simTime - servingPacket.generatedTime; 
        refToDestination++;
        refDeparts++;
    }
    sumPacketDelay += simTime - servingPacket.arrivalTime;
    queueDelay[servingPacket.type] += simTime - servingPacket.arrivalTime;
    numPacketTransmitted++;
    queueTransmitted[servingPacket.type]++;

    if (!premiumQueue.isEmpty()) {
        servingPacket = premiumQueue.dequeue();
        numPackets++;   
        nextDepartureTime = simTime + servingPacket.serviceTime;
    } else if (!assuredQueue.isEmpty()) {
        servingPacket = assuredQueue.dequeue();
        numPackets++;   
        nextDepartureTime = simTime + servingPacket.serviceTime;
    } else if (!bestEffortQueue.isEmpty()) {
        servingPacket = bestEffortQueue.dequeue();
        numPackets++;
        nextDepartureTime = simTime + servingPacket.serviceTime;
    }else if (bestEffortQueue.isEmpty()) {
        // All queues are empty
        status = IDLE;
        nextDepartureTime = std::numeric_limits<double>::max();
    }
}

double Node::getNextOnTime() {
    double min_on_time = std::numeric_limits<double>::max();
    int type = 0;
    for (auto& sourcesOfType : getSources()) {
        for (auto& source : sourcesOfType) {           
            double on_time = source.getNextOnTime();
            if (on_time < min_on_time) {
                min_on_time = on_time;
				nextOnType = static_cast<PacketType>(type);
				nextOnIndex = source.getId();
            }
        }
		type++;
    }
	return min_on_time;
}

double Node::getNextOffTime() {
    double min_off_time = std::numeric_limits<double>::max();
    int type = 0;
    for (auto& sourcesOfType : getSources()) {
        for (auto& source : sourcesOfType) {
            double off_time = source.getNextOffTime();
            if (off_time < min_off_time) {
                min_off_time = off_time;
				nextOffType = static_cast<PacketType>(type);
                nextOffIndex = source.getId();
            }
        }
        type++;
    }
	return min_off_time;
}

double Node::getNextArrivalTime() {
    int type = 0;
    nextArrivalTime = std::numeric_limits<double>::max();
    for (auto& typeSources : getSources()) {
        for (auto& src : typeSources) {
            if (src.getStatus() == Source::Status::ON) {
                double arrivalTime = src.getnextPacketTime();
                if (arrivalTime < nextArrivalTime) {
                    nextArrivalTime = arrivalTime;
					nextArriveIndex = src.getId();
					nextArriveType = static_cast<PacketType>(type);
                }
            }
        }
		type++;
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

/*void Node::updateSources() {
	sources = { audioSources, videoSources, dataSources };
} */

std::vector<std::vector<Source>> Node::getSources() {
    return { audioSources, videoSources, dataSources };
}

Source* Node::getSource(PacketType type, int index) {
    switch (type) {
    case AUDIO:
        if (index >= 0 && index < audioSources.size()) {
            return &audioSources[index];
        }
        break;
    case VIDEO:
        if (index >= 0 && index < videoSources.size()) {
            return &videoSources[index];
        }
        break;
    case DATA:
        if (index >= 0 && index < dataSources.size()) {
            return &dataSources[index];
        }
        break;
    default:
        throw std::runtime_error("Invalid PacketType");
    }
    return nullptr;
}