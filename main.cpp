#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include "System.h"
#include "globals.h"

// System components
int numNodes = 3;
PacketType refType = PacketType::AUDIO;
int numBackgroundAudioSources = 4;
int numBackgroundVideoSources = 4;
int numBackgroundDataSources = 4;
int buffer_size = 100;
Source referenceSource;
std::vector<Node> nodes;
Source* next_on_source;
Source* next_off_source;
Source* next_arrival_source;
Node* next_departure_node;

// Constants
static const int Q_LIMIT = 100;  // Limit on queue length
static const int BUSY = 1;       // Mnemonics for server's being busy
static const int IDLE = 0;       // and idle
enum EventType {
	ARRIVAL,
	DEPARTURE,
    ON,
	OFF
};
struct SourceEvent
{
	EventType type;
	double time;
	int source;
};
struct PacketEvent;

// State variables
EventType next_event_type;
int num_custs_delayed;
int num_delays_required;
const int num_events = 4;



// Statistical counters
double area_num_in_q;
double area_server_status;
double sim_time;
double time_arrival[Q_LIMIT + 1]; //queue
double time_last_event;
double time_next_event[num_events];
double total_of_delays;

// Random number generation
std::mt19937 gen;
std::exponential_distribution<> exp_dist;

// File streams
std::ifstream infile;
std::ofstream outfile;

// Arival event statistics
double next_arrival_time; //A
double clock;
double area_under_bt;
int dropped;


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
    while (num_custs_delayed < buffer_size) {
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

void initialize() {
    // Initialize simulation clock
    sim_time = 0.0;

    gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

    // Initialize System
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
    
    // Initialize state variables
    // server_status = IDLE;
    // num_in_q = 0;
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
    double min_time_next_event = std::numeric_limits<double>::max();
    next_event_type = ON;

    // Determine the event type of the next event to occur
    for (int i = 1; i <= num_events; ++i) {
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
    time_next_event[1] = ;
    next_arrival_time = time_next_event[1];


    // Check if server is busy
    if (server_status == BUSY) {


        if (num_in_q > Q_LIMIT) {
            dropped++;
            // throw std::runtime_error("Queue overflow");
        }
        else {
            // Store arrival time of arriving customer at end of queue
            time_arrival[num_in_q] = sim_time;

            // Server is busy, increment queue
            ++num_in_q;
        }

    }
    else {
        // Server is idle, so arriving customer has zero delay
        double delay = 0.0;
        total_of_delays += delay;

        // Increment number of customers delayed and make server busy
        ++num_custs_delayed;
        server_status = BUSY;

        // Schedule a departure (service completion)
        time_next_event[2] = sim_time + expon(mean_service);
    }
}

void depart() {
    // Check if queue is empty
    if (num_in_q == 0) {
        // Queue is empty, make server idle and eliminate departure event
        server_status = IDLE;
        time_next_event[2] = 1.0e+30;
    }
    else {
        // Queue is not empty, process next customer
        --num_in_q;

        // Compute delay of customer who is beginning service
        double delay = sim_time - time_arrival[1];
        total_of_delays += delay;  //attribute for node

        // Increment number of customers delayed and schedule departure
        ++num_custs_delayed;
        time_next_event[2] = sim_time + expon(mean_service);

        // Move each customer in queue up one position
        if (num_in_q > 0) {
            for (int i = 1; i <= num_in_q; ++i) {
                time_arrival[i] = time_arrival[i + 1];
            }
        } // If there are multiple queues this will need to be changed
    }
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
    for (auto& node : nodes) {
        double off_time = node.nextOffTime();
        if (off_time < min_off_time) {
            min_off_time = off_time;
        }
    }
    return min_off_time;
}

double expon(double mean) {
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

void report() {
    outfile << "\nSimulation Report:\n"
        << "Average delay in queue: " << total_of_delays / num_custs_delayed << " minutes\n"
        << "Average number in queue: " << area_num_in_q / sim_time << "\n"
        << "Server utilization: " << area_server_status / sim_time << "\n"
        << "Time simulation ended: " << sim_time << " minutes\n";
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