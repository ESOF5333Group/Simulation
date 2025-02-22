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

#endif // CONFIG_H
