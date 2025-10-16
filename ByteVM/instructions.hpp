#pragma once

#include <cstdint>

enum class instructions : uint8_t {
    gvm_push,
    gvm_add,
    gvm_store,
    gvm_load,
    gvm_call,
    gvm_label,
    gvm_function,
    gvm_ret,
};


