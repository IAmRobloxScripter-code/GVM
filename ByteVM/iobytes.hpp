#pragma once

#include <cstddef>
#include <ios>
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <vector>
#include <cstring>
#include "instructions.hpp"
#include "object.h"

template <typename T>
void write_bytes(std::vector<uint8_t> &f, T value) {
    const char* bytes = reinterpret_cast<const char*>(&value);
    f.insert(f.end(), bytes, bytes + sizeof(T));
}

template<typename T>
T bytes_to_number(uint8_t* bytes);

