// filepath: /c:/Users/Han/source/repos/ConsoleApplication1/globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "Source.h"
#include "Node.h"
#include "config.h"

extern std::vector<Node> nodes;
extern double simTime;

// extern double transmissionRate;
extern int numPackets;
extern int referenceCounter;

extern int totalGenerated;

extern int numSuccessTransmitted;

extern int dropped;

#endif // GLOBALS_H