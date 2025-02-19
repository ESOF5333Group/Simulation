    #ifndef SYSTEM_H
#define SYSTEM_H

#include "Node.h"
#include "Source.h"
#include <vector>

class System {
public:
    System(int numNodes, PacketType refType, int numBackgroundAudioSources, int numBackgroundVideoSources, int numBackgroundDataSources);
    int bufferUsage = 0;

private:
    std::vector<Node> nodes;
    std::vector<Source> backgroundSources;
    Source referenceSource;
    int numBackgroundAudioSources;
    int numBackgroundVideoSources;
    int numBackgroundDataSources;

};

#endif // SYSTEM_H

