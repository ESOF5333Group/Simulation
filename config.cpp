#include "config.h"
#include <cmath>

Config audioConfig = { PacketType::AUDIO, 64000, 0.36, 0.64, 120, 30 }; // kbps, sec, sec, bytes, number of sources
Config videoConfig = { PacketType::VIDEO, 384000, 0.33, 0.73, 1000, 30 }; // kbps, sec, sec, bytes, number of sources
Config dataConfig = { PacketType::DATA, 256000, 0.35, 0.65, 583, 30 }; // kbps, sec, sec, bytes, number of sources

int numNodes = 5;

int numBackgroundAudioSources = 36;
int numBackgroundVideoSources = 38;
int numBackgroundDataSources = 29;

int multipleNumsAudio[9] = { 5, 10, 15, 20, 25, 29, 25, 37, 41};
int multipleNumsVideo[9] = { 5, 9, 14, 20, 24, 29, 35, 38, 45 };
int multipleNumsData[9] = { 4, 8, 11, 13, 18, 21, 25, 29, 30 };

PacketType refType = PacketType::VIDEO;

double transmissionRate = 1e7; // bps

int numPacketsRequired = 1000000;

int spqSize = 100;