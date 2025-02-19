#ifndef QUEUE_H
#define QUEUE_H

#include "Node.h"
#include "Source.h"
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

class Queue {
public:
    Queue(int numNodes, int numBackgroundSources, int numPackets);
    void run();
    int num_in_q;
    void depart();

private:
    // Constants
    static const int Q_LIMIT = 100;  // Limit on queue length
    static const int BUSY = 1;       // Mnemonics for server's being busy
    static const int IDLE = 0;       // and idle

    // State variables
    int next_event_type;
    int num_custs_delayed;
    int num_delays_required;
    int num_events;

    int server_status;

    // Statistical counters
    double area_num_in_q;
    double area_server_status;
    double mean_interarrival;
    double mean_service;
    double sim_time;
    double time_arrival[Q_LIMIT + 1];
    std::queue <Packet> queue;
    
    double time_last_event;
    double time_next_event[3];
    double total_of_delays;

    // Random number generation
    std::mt19937 gen;
    std::exponential_distribution<> exp_dist;

    // File streams
    std::ifstream infile;
    std::ofstream outfile;

    // Private member functions
    void initialize();
    void timing();
    void arrive();

    void report();
    void update_time_avg_stats();
    double expon(double mean);

    std::vector<Node> nodes;
    std::vector<Source> backgroundSources;
    Source referenceSource;
    int numPackets;

    Packet serving;
};

#endif // QUEUE_H
