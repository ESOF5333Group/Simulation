#include "config.h"

Config audioConfig = { PacketType::AUDIO, 64000, 0.36, 0.64, 120, 4 }; // kbps, sec, sec, bytes, number of sources
Config videoConfig = { PacketType::VIDEO, 384000, 0.33, 0.73, 1000, 4 }; // kbps, sec, sec, bytes, number of sources
Config dataConfig = { PacketType::DATA, 256000, 0.35, 0.65, 583, 4 }; // kbps, sec, sec, bytes, number of sources
