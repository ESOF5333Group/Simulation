#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include "globals.h"
#include "config.h"
#include "Queue.h"
#include "Node.h"
#include "Source.h"


// System components
int numNodes = 5;
PacketType refType = PacketType::AUDIO;
int numBackgroundAudioSources = 4;
int numBackgroundVideoSources = 5;
int numBackgroundDataSources = 4;
Source referenceSource;
std::vector<Node> nodes;
Source* next_on_source;
Source* next_off_source;
Source* next_packet_source;
Node* next_arrival_node;
Node* next_departure_node;

double transmissionRate = 1e7; // bps

enum EventType {
	NONE,
    ARRIVAL,
	DEPARTURE,
    ON,
	OFF
};

// State variables
EventType next_event_type;

const int num_events = 5;
double sim_time;
double time_last_event;
double time_next_event[num_events];

// Statistical counters
int num_custs_delayed;
int num_delays_required = 100000;
double area_num_in_q;
double area_server_status;

double total_of_delays;
int referenceCounter;

int totalGenerated;

int successfully_transmitted_packets;

// Random number generation
std::mt19937 gen;
std::exponential_distribution<> exp_dist;

// File streams
std::ifstream infile;
std::ofstream outfile;

// Arival event statistics
double area_under_bt;
int dropped;

double next_arrival_time() {
    double min_arrival_time = referenceSource.getnextPacketTime();
    next_packet_source = &referenceSource;
	next_arrival_node = &nodes[0];
    for (auto& node : nodes) {
        double arrival_time = node.getNextArrivalTime();
        if (arrival_time < min_arrival_time) {
            min_arrival_time = arrival_time;
            next_arrival_node = &node;
        }
    }
    return min_arrival_time;
}

double next_departure_time() {
    double min_departure_time = std::numeric_limits<double>::max();
    for (auto& node : nodes) {
        double departure_time = node.getNextDepartureTime();
        if (departure_time < min_departure_time) {
            min_departure_time = departure_time;
            next_departure_node = &node;
        }
    }
    return min_departure_time;
}

double next_on_time() {
    double min_on_time = referenceSource.getNextOnTime();
    next_on_source = &referenceSource;
    for (auto& node : nodes) {
        double on_time = node.nextOnTime();
        if (on_time < min_on_time) {
            min_on_time = on_time;
        }
    }
    return min_on_time;
}

double next_off_time() {
    double min_off_time = referenceSource.getNextOffTime();
    next_off_source = &referenceSource;
    for (auto& node : nodes) {
        double off_time = node.nextOffTime();
        if (off_time < min_off_time) {
            min_off_time = off_time;
        }
    }
    return min_off_time;
}

void initialize() {
    // Initialize simulation clock
    sim_time = 0.0;

	referenceCounter = 0;
	totalGenerated = 0;
	successfully_transmitted_packets = 0;

    gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

    // Initialize System
    referenceSource = Source(0, audioConfig, true);
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
    
    // Initialize state variables
    time_last_event = 0.0;

    // Initialize statistical counters
    num_custs_delayed = 0;
    total_of_delays = 0.0;
    area_num_in_q = 0.0;
    area_server_status = 0.0;

    // Initialize event list
    time_next_event[EventType::ARRIVAL] = std::numeric_limits<double>::max();
    time_next_event[EventType::DEPARTURE] = std::numeric_limits<double>::max();
    time_next_event[EventType::ON] = next_on_time();
    time_next_event[EventType::OFF] = std::numeric_limits<double>::max();

    dropped = 0;
}

void timing() {
    double min_time_next_event = 1.0e+29;
    next_event_type = NONE;

    time_next_event[ARRIVAL] = next_arrival_time();
    time_next_event[DEPARTURE] = next_departure_time();
    time_next_event[ON] = next_on_time();
    time_next_event[OFF] = next_off_time();

    min_time_next_event = time_next_event[1];
    next_event_type = ARRIVAL;

    // Determine the event type of the next event to occur
    for (int i = 1; i < num_events; ++i) {
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = static_cast<EventType>(i);
        }
    }

    // Check if event list is empty
    if (next_event_type == 0) {
        outfile << "\nEvent list empty at time " << sim_time;
        throw std::runtime_error("Event list empty");
    }

    sim_time = min_time_next_event;
}

void arrive() {
    // Schedule next arrival

    next_arrival_node->arrive(next_packet_source->nextPacket());

}

void depart() {
    next_departure_node->depart();
}

void switchon() {
    // Implement the switchon functionality here
    if (next_on_source) {
        next_on_source->switchOn(sim_time);
    }
    time_next_event[EventType::ON] = next_on_time();
}

void switchoff() {
    // Implement the switchoff functionality here
    if (next_off_source) {
        next_off_source->switchOff(sim_time);
    }
    time_next_event[EventType::OFF] = next_off_time();
}

void update_time_avg_stats() {
    double time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;

    // Update area under number-in-queue function
    // area_num_in_q += num_in_q * time_since_last_event;

    // Update area under server-busy indicator function
    // area_server_status += server_status * time_since_last_event;
}

double expon(double mean) {
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

void report() {
    outfile << "\nSimulation Report:\n"
        //<< "Average delay in queue: " << total_of_delays / num_custs_delayed << " minutes\n"
        //<< "Average number in queue: " << area_num_in_q / sim_time << "\n"
        //<< "Server utilization: " << area_server_status / sim_time << "\n"
        << "Time simulation ended: " << sim_time << " minutes\n"
	    << "Node 1 average packet delay: " << nodes[0].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 2 average packet delay: " << nodes[1].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 3 average packet delay: " << nodes[2].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 4 average packet delay: " << nodes[3].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 5 average packet delay: " << nodes[4].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n";

}

void run(const std::string& input_file, const std::string& output_file) {
    // Open input and output files
    infile.open(input_file);
    outfile.open(output_file);

    if (!infile.is_open() || !outfile.is_open()) {
        throw std::runtime_error("Error opening files");
    }

    // Write report heading and input parameters
    outfile << "Single-server queuing system\n\n"
        << "Number of customers: " << num_delays_required << "\n\n";

    // Initialize the simulation
    initialize();

    // Run the simulation while more delays are still needed
    while (num_custs_delayed < num_delays_required) {
        timing();
        update_time_avg_stats();

        // Invoke the appropriate event function
        switch (next_event_type) {
        case ARRIVAL:
            arrive();
            break;
        case DEPARTURE:
            depart();
            break;
        case ON:
            switchon();
            break;
        case OFF:
            switchoff();
            break;
        }
    }

    // Generate report and clean up
    report();
    infile.close();
    outfile.close();
}

// Example main function
int main() {
    try {
        run("mm1.in", "mm1.out");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}