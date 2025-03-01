#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "Source.h"
#include "Node.h"
#include "config.h"

extern std::vector<Node> nodes;
extern double simTime;

extern int numPackets;

extern int totalGenerated;
extern int refGenerated;
extern int typeGenerated[3];

extern int typeDropped[3];
extern int typeBacklogged[3];

extern int refDeparts;
extern int dropped;
extern int refDropped;
extern int refToDestination;

extern double refSumDelay;

#endif // GLOBALS_H