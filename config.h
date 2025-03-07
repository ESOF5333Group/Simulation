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

extern int multipleNumsAudio[9];
extern int multipleNumsVideo[9];
extern int multipleNumsData[9];

extern PacketType refType;

extern double transmissionRate; // bps

extern int numPacketsRequired;

extern int spqSize;

#endif // CONFIG_H
