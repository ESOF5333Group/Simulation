#include "config.h"

Config audioConfig = { PacketType::AUDIO, 64000, 0.36, 0.64, 120, 30 }; // kbps, sec, sec, bytes, number of sources
Config videoConfig = { PacketType::VIDEO, 384000, 0.33, 0.73, 1000, 30 }; // kbps, sec, sec, bytes, number of sources
Config dataConfig = { PacketType::DATA, 256000, 0.35, 0.65, 583, 30 }; // kbps, sec, sec, bytes, number of sources

int numNodes = 5;

int numBackgroundAudioSources = 50;
int numBackgroundVideoSources = 50;
int numBackgroundDataSources = 30;

PacketType refType = PacketType::VIDEO;

double transmissionRate = 1e7; // bps

int numPacketsRequired = 100000;