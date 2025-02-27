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
Source referenceSource;

std::vector<Node> nodes;

Node* nextOnNode;
Node* nextOffNode;
bool isNextOnRef = true;
bool isNextOffRef = true;

Node* nextArriveNode;
Node* nextDepartNode;
bool isNextArriveRef = true;

enum EventType {
	NONE,
    ARRIVAL,
	DEPARTURE,
    ON,
	OFF
};

// State variables
EventType nextEventType;
const int NUM_EVENTS = 5;
double simTime;
double timeLastEvent;
double timeNextEvent[NUM_EVENTS];

bool isStateCheckNeeded = true;

// Statistical counters
int numPackets;

double areaNumInQ;
double areaServerStatus;

double totalOfDelays;
int referenceCounter;

int totalGenerated;

int numSuccessTransmitted;

// File streams
std::ifstream infile;
std::ofstream outfile;

// Arival event statistics
int dropped = 0;

double getNextArriveTime() {
    double minArrivalTime = referenceSource.getnextPacketTime();
	isNextArriveRef = true;
	nextArriveNode = &nodes[0];
    for (auto& node : nodes) {
        double arrivalTime = node.getNextArrivalTime();
        if (arrivalTime < minArrivalTime) {
            minArrivalTime = arrivalTime;
            nextArriveNode = &node;
			isNextArriveRef = false;
        }
    }
    return minArrivalTime;
}

double getNextDepartTime() {
    double minDepartTime = std::numeric_limits<double>::max();
    for (auto& node : nodes) {
        double departTime = node.getNextDepartureTime();
        if (departTime < minDepartTime) {
            minDepartTime = departTime;
            nextDepartNode = &node;
        }
    }
    return minDepartTime;
}

double getNextOnTime() {
    double min_on_time = referenceSource.getNextOnTime();

	isNextOnRef = true;
    for (auto& node : nodes) {
        double on_time = node.getNextOnTime();
        if (on_time < min_on_time) {
            min_on_time = on_time;
			nextOnNode = &node;
			isNextOnRef = false;
        }
    }
    return min_on_time;
}

double getNextOffTime() {
    double min_off_time = referenceSource.getNextOffTime();

    isNextOffRef = true;
    for (auto& node : nodes) {
        double off_time = node.getNextOffTime();
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
    simTime = 0.0;

	referenceCounter = 0;
	totalGenerated = 0;
	numSuccessTransmitted = 0;

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
    timeLastEvent = 0.0;

    // Initialize statistical counters
    numPackets = 0;
    totalOfDelays = 0.0;
    areaNumInQ = 0.0;
    areaServerStatus = 0.0;

    // Initialize event list
    timeNextEvent[EventType::ARRIVAL] = std::numeric_limits<double>::max();
    timeNextEvent[EventType::DEPARTURE] = std::numeric_limits<double>::max();
    timeNextEvent[EventType::ON] = getNextOnTime();
    timeNextEvent[EventType::OFF] = std::numeric_limits<double>::max();
}

void timing() {
    double min_time_next_event = 1.0e+29;
    nextEventType = NONE;

    if (isStateCheckNeeded) {

        timeNextEvent[ON] = getNextOnTime();
        timeNextEvent[OFF] = getNextOffTime();
        isStateCheckNeeded = false;
    }

    timeNextEvent[ARRIVAL] = getNextArriveTime();
    timeNextEvent[DEPARTURE] = getNextDepartTime();

    min_time_next_event = timeNextEvent[1];
    nextEventType = ARRIVAL;

    // Determine the event type of the next event to occur
    for (int i = 1; i < NUM_EVENTS; ++i) {
        if (timeNextEvent[i] < min_time_next_event) {
            min_time_next_event = timeNextEvent[i];
            nextEventType = static_cast<EventType>(i);
        }
    }

    // Check if event list is empty
    if (nextEventType == 0) {
        outfile << "\nEvent list empty at time " << simTime;
        throw std::runtime_error("Event list empty");
    }

    simTime = min_time_next_event;
}

void arrive() {
    // Schedule next arrival
	if (isNextArriveRef) {
        nextArriveNode->arrive(referenceSource.nextPacket());
	}
	else {
		nextArriveNode->arrive();
	}   
}

void depart() {
    nextDepartNode->depart();
}

void switchon() {
    if (isNextOnRef) {
        referenceSource.switchOn(simTime);
	}
    else {
        nextOnNode->switchNextOn(simTime);
    }
	isStateCheckNeeded = true;
    //time_next_event[EventType::ON] = next_on_time();
}

void switchoff() {
    if (isNextOffRef) {
        referenceSource.switchOff(simTime);
    }
    else {
        nextOffNode->switchNextOff(simTime);
    }
	isStateCheckNeeded = true;
    //time_next_event[EventType::OFF] = next_off_time();
}

void update_time_avg_stats() {
    double time_since_last_event = simTime - timeLastEvent;
    timeLastEvent = simTime;

    // Update area under number-in-queue function
    // area_num_in_q += num_in_q * time_since_last_event;

    // Update area under server-busy indicator function
    // area_server_status += server_status * time_since_last_event;
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
        << "Time simulation ended: " << simTime << " seconds\n"
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
		<< "Successfully transmitted packets: " << numSuccessTransmitted << "\n";

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
    // infile.open(input_file);
    outfile.open(output_file, std::ios::app);

    if (!outfile.is_open()) {
        throw std::runtime_error("Error opening files");
    }

    // Write report heading and input parameters
    outfile << "SPQ system\n\n"
        << "Number of packets: " << numPacketsRequired << "\n\n";

    // Initialize the simulation
    initialize();

    // Run the simulation while more delays are still needed
    while (numPackets < numPacketsRequired) {
        timing();
        update_time_avg_stats();

        // Invoke the appropriate event function
        switch (nextEventType) {
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
        run("scenario1.in");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}