#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include <filesystem> // For creating directories
#include <iomanip>    // For formatting the timestamp
#include <sstream>

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
Node* nextOnNode;
Node* nextOffNode;
bool isNextOnRef = true;
bool isNextOffRef = true;
Source* next_packet_source;

Node* next_arrival_node;
Node* next_departure_node;
PacketType next_on_type;
PacketType next_off_type;
int next_on_index;
int next_off_index;
bool isNextArriveRef = true;

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

const int NUM_EVENTS = 5;
double sim_time;
double time_last_event;
double time_next_event[NUM_EVENTS];

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
	isNextArriveRef = true;
	next_arrival_node = &nodes[0];
    for (auto& node : nodes) {
        double arrival_time = node.getNextArrivalTime();
        if (arrival_time < min_arrival_time) {
            min_arrival_time = arrival_time;
            next_arrival_node = &node;
			isNextArriveRef = false;
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
	isNextOnRef = true;
    for (auto& node : nodes) {
        double on_time = node.nextOnTime();
        if (on_time < min_on_time) {
            min_on_time = on_time;
			nextOnNode = &node;
			isNextOnRef = false;
        }
    }
    return min_on_time;
}

double next_off_time() {
    double min_off_time = referenceSource.getNextOffTime();
    next_off_source = &referenceSource;
    isNextOffRef = true;
    for (auto& node : nodes) {
        double off_time = node.nextOffTime();
        if (off_time < min_off_time) {
            min_off_time = off_time;
            nextOffNode = &node;
            isNextOffRef = false;
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

    gen.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));

    // Initialize System
    referenceSource = Source(0, audioConfig, true);
    for (int i = 0; i < numNodes; ++i) {
        nodes.emplace_back(i);
        // Create audio sources
        for (int j = 0; j < numBackgroundAudioSources; ++j) {
            nodes[i].audioSources.emplace_back(j, audioConfig, false);
        }
        // Create video sources
        for (int j = 0; j < numBackgroundVideoSources; ++j) {
            nodes[i].videoSources.emplace_back(j, videoConfig, false);
        }
        // Create data sources
        for (int j = 0; j < numBackgroundDataSources; ++j) {
            nodes[i].dataSources.emplace_back(j, dataConfig, false);
        }
		// nodes[i].updateSources();
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
    for (int i = 1; i < NUM_EVENTS; ++i) {
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
	if (isNextArriveRef) {
        next_arrival_node->arrive(referenceSource.nextPacket());
	}
	else {
		next_arrival_node->arrive();
	}
    
}

void depart() {
    next_departure_node->depart();
}

void switchon() {
    if (isNextOnRef) {
        referenceSource.switchOn(sim_time);
	}
    else {
        nextOnNode->switchNextOn(sim_time);
    }
    time_next_event[EventType::ON] = next_on_time();
}

void switchoff() {
    if (isNextOffRef) {
        referenceSource.switchOff(sim_time);
    }
    else {
        nextOffNode->switchNextOff(sim_time);
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
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char time_str[26];
    ctime_s(time_str, sizeof(time_str), &now_time);

    outfile << "\nSimulation Report:\n"
        //<< "Average delay in queue: " << total_of_delays / num_custs_delayed << " minutes\n"
        //<< "Average number in queue: " << area_num_in_q / sim_time << "\n"
        //<< "Server utilization: " << area_server_status / sim_time << "\n"
        << "Timestamp: " << time_str
        << "Time simulation ended: " << sim_time << " minutes\n"
		<< "Number of nodes: " << numNodes << "\n"
		<< "Number of background audio sources: " << numBackgroundAudioSources << "\n"
		<< "Number of background video sources: " << numBackgroundVideoSources << "\n"
		<< "Number of background data sources: " << numBackgroundDataSources << "\n"
		<< "Reference packet type: " << (refType == PacketType::AUDIO ? "Audio" : "Video") << "\n"
	    << "Node 1 average packet delay: " << nodes[0].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 2 average packet delay: " << nodes[1].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 3 average packet delay: " << nodes[2].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 4 average packet delay: " << nodes[3].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n"
        << "Node 5 average packet delay: " << nodes[4].getSumPacketDelay() / nodes[0].getNumPacketTransmitted() << " seconds\n";
	// Keep track of sources, in each source how many packets were generated, for each node number of packets into and out of.
    // Print reference packets number, dropped number of ref. 

    for (size_t i = 0; i < nodes.size(); ++i) {
        outfile << "Node " << i + 1 << " average packet delay: "
            << nodes[i].getSumPacketDelay() / nodes[i].getNumPacketTransmitted() << " seconds\n"
            << "Node " << i + 1 << " packets into node: " << nodes[i].getNumPacketArrive() << "\n"
            << "Node " << i + 1 << " packets out of node: " << nodes[i].getNumPacketTransmitted() << "\n";
    }

	outfile << "Reference packets transmitted time: " << referenceCounter << "\n"
		<< "Dropped packets: " << dropped << "\n"
		<< "Total generated packets: " << totalGenerated << "\n"
		<< "Successfully transmitted packets: " << successfully_transmitted_packets << "\n";

    for (size_t i = 0; i < nodes.size(); ++i) {
        outfile << "Node " << i + 1 << " source packet generation:\n";
        for (const auto& source : nodes[i].audioSources) {
            outfile << "  Audio Source " << source.getId() << ": " << source.getGeneratedPackets() << " packets generated\n";
        }
        for (const auto& source : nodes[i].videoSources) {
            outfile << "  Video Source " << source.getId() << ": " << source.getGeneratedPackets() << " packets generated\n";
        }
        for (const auto& source : nodes[i].dataSources) {
            outfile << "  Data Source " << source.getId() << ": " << source.getGeneratedPackets() << " packets generated\n";
        }
    }
}

void run(const std::string& input_file) {
    // Create the report directory if it doesn't exist
    std::filesystem::create_directory("report");
    
    // Generate a timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_time);

    // Create a filename with the timestamp
    std::stringstream reportFile;
    reportFile << "report/" << std::put_time(&now_tm, "%Y%m%d_%H%M%S") << ".out";
    std::string output_file = reportFile.str();

    // Open input and output files
    infile.open(input_file);
    outfile.open(output_file, std::ios::app);

    if (!infile.is_open() || !outfile.is_open()) {
        throw std::runtime_error("Error opening files");
    }

    // Write report heading and input parameters
    outfile << "SPQ system\n\n"
        << "Number of packets: " << num_delays_required << "\n\n";

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
        run("mm1.in");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}