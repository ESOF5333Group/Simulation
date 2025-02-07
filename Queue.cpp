#include "Queue.h"
#include <iostream>

Queue::Queue(int numNodes, int numBackgroundSources, int numPackets)
    : referenceSource(0, 0.36, 0.64, 120, 64), numPackets(numPackets) {
    for (int i = 0; i < numNodes; ++i) {
        nodes.emplace_back();

    }
    for (int i = 1; i <= numBackgroundSources; ++i) {
        backgroundSources.emplace_back(i, 0.36, 0.64, 120, 64);
    }
}

void Queue::initialize() {
    // Initialize simulation clock
    sim_time = 0.0;

    // Initialize state variables
    server_status = IDLE;
    num_in_q = 0;
    time_last_event = 0.0;

    // Initialize statistical counters
    num_custs_delayed = 0;
    total_of_delays = 0.0;
    area_num_in_q = 0.0;
    area_server_status = 0.0;

    // Initialize event list
    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}

void Queue::timing() {
    double min_time_next_event = 1.0e+29;
    next_event_type = 0;

    // Determine the event type of the next event to occur
    for (int i = 1; i <= num_events; ++i) {
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }
    }

    // Check if event list is empty
    if (next_event_type == 0) {
        outfile << "\nEvent list empty at time " << sim_time;
        throw std::runtime_error("Event list empty");
    }

    sim_time = min_time_next_event;
}

void Queue::arrive() {
    // Schedule next arrival
    time_next_event[1] = sim_time + expon(mean_interarrival);

    // Check if server is busy
    if (server_status == BUSY) {
        // Server is busy, increment queue
        ++num_in_q;

        if (num_in_q > Q_LIMIT) {
            throw std::runtime_error("Queue overflow");
        }

        // Store arrival time of arriving customer at end of queue
        time_arrival[num_in_q] = sim_time;
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

void Queue::depart() {
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
        total_of_delays += delay;

        // Increment number of customers delayed and schedule departure
        ++num_custs_delayed;
        time_next_event[2] = sim_time + expon(mean_service);

        // Move each customer in queue up one position
        for (int i = 1; i <= num_in_q; ++i) {
            time_arrival[i] = time_arrival[i + 1];
        }
    }
}

void Queue::update_time_avg_stats() {
    double time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;

    // Update area under number-in-queue function
    area_num_in_q += num_in_q * time_since_last_event;

    // Update area under server-busy indicator function
    area_server_status += server_status * time_since_last_event;
}

double Queue::expon(double mean) {
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

void Queue::report() {
    outfile << "\nSimulation Report:\n"
        << "Average delay in queue: " << total_of_delays / num_custs_delayed << " minutes\n"
        << "Average number in queue: " << area_num_in_q / sim_time << "\n"
        << "Server utilization: " << area_server_status / sim_time << "\n"
        << "Time simulation ended: " << sim_time << " minutes\n";
}
void Queue::run() {
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

static double generateR(double m) {
	double u = static_cast<double>(rand()) / RAND_MAX;

	double r = -log(1 - u) / m;

	return r;
}
