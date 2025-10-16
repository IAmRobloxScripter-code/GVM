#pragma once
#include <cstdint>
#include <fstream>

struct header {
    uint32_t identifier = 0x47564D00;
    uint64_t version_major = 0;
    uint64_t version_minor = 1;
};

struct program {
    std::vector<uint8_t> _header;
    std::vector<uint8_t> _prototypes;
    std::vector<uint8_t> _main;
};