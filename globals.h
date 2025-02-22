// filepath: /c:/Users/Han/source/repos/ConsoleApplication1/globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "Source.h"
#include "Node.h"
#include "config.h"

extern std::vector<Node> nodes;
extern int numNodes;
extern double sim_time;
extern Source* next_on_source;
extern Source* next_off_source;
extern Source* next_packet_source;


extern double transmissionRate;
extern int num_custs_delayed;
extern int referenceCounter;

extern int totalGenerated;

extern int successfully_transmitted_packets;

#endif // GLOBALS_H