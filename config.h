#ifndef CONFIG_H
#define CONFIG_H
enum PacketType
{
    AUDIO,
    VIDEO,
    DATA
};

struct Config {
    PacketType type;
    int peakBitRate; // kbps
    double meanOnTime; // sec
    double meanOffTime; // sec
    int packetSize; // bytes
    int numSources; // number of sources
};

extern Config audioConfig; // kbps, sec, sec, bytes, number of sources
extern Config videoConfig; // kbps, sec, sec, bytes, number of sources
extern Config dataConfig; // kbps, sec, sec, bytes, number of sources

extern int numNodes;

extern int numBackgroundAudioSources;
extern int numBackgroundVideoSources;
extern int numBackgroundDataSources;

extern PacketType refType;

extern double transmissionRate; // bps

extern int numPacketsRequired;

#endif // CONFIG_H
