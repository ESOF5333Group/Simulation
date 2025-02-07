#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

class QueueSimulation {
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
    int num_in_q;
    int server_status;

    // Statistical counters
    double area_num_in_q;
    double area_server_status;
    double mean_interarrival;
    double mean_service;
    double sim_time;
    double time_arrival[Q_LIMIT + 1]; //queue
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
    void depart();
    void report();
    void update_time_avg_stats();
    double expon(double mean);

    // Arival event statistics
    double next_arrival_time; //A
    double clock;
    double area_under_bt;
    int dropped;


public:
    QueueSimulation() : gen(std::chrono::system_clock::now().time_since_epoch().count()) {
        num_events = 2;
    }

    void run(const std::string& input_file, const std::string& output_file) {
        // Open input and output files
        infile.open(input_file);
        outfile.open(output_file);

        if (!infile.is_open() || !outfile.is_open()) {
            throw std::runtime_error("Error opening files");
        }

        // Read input parameters
        infile >> mean_interarrival >> mean_service >> num_delays_required;

        // Write report heading and input parameters
        outfile << "Single-server queuing system\n\n"
            << "Mean interarrival time: " << mean_interarrival << " minutes\n"
            << "Mean service time: " << mean_service << " minutes\n"
            << "Number of customers: " << num_delays_required << "\n\n";

        // Initialize the simulation
        initialize();

        // Run the simulation while more delays are still needed
        while (num_custs_delayed < num_delays_required) {
            timing();
            update_time_avg_stats();

            // Invoke the appropriate event function
            switch (next_event_type) {
            case 1:
                arrive();
                break;
            case 2:
                depart();
                break;
            }
        }

        // Generate report and clean up
        report();
        infile.close();
        outfile.close();
    }
};

void QueueSimulation::initialize() {
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

    dropped = 0;
}

void QueueSimulation::timing() {
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

void QueueSimulation::arrive() {
    // Schedule next arrival
    time_next_event[1] = sim_time + expon(mean_interarrival);
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

void QueueSimulation::depart() {
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

void QueueSimulation::update_time_avg_stats() {
    double time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;

    // Update area under number-in-queue function
    area_num_in_q += num_in_q * time_since_last_event;

    // Update area under server-busy indicator function
    area_server_status += server_status * time_since_last_event;
}

double QueueSimulation::expon(double mean) {
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

void QueueSimulation::report() {
    outfile << "\nSimulation Report:\n"
        << "Average delay in queue: " << total_of_delays / num_custs_delayed << " minutes\n"
        << "Average number in queue: " << area_num_in_q / sim_time << "\n"
        << "Server utilization: " << area_server_status / sim_time << "\n"
        << "Time simulation ended: " << sim_time << " minutes\n";
}

// Example main function
int main() {
    try {
        QueueSimulation sim;
        sim.run("mm1.in", "mm1.out");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}