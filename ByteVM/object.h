#pragma once
#include <string>
#define GC_TRIGGER_MEMORY_COUNT_MB 100

enum class object_types : uint8_t
{
    number,
    string,
    boolean,
    function
};

struct object;

struct stack_frame
{
    std::vector<object *> locals;
    stack_frame *last = nullptr;
    uint8_t gc_color = 2;
};

struct function
{
    std::vector<object *> upvalues;

    uint8_t params;
    uint64_t ip;
    std::string name;
    uint64_t header_size;
};

struct object
{
    object_types object_type;
    union
    {
        double number;
        std::string string;
        bool boolean;
        function *func;
    };
    uint8_t gc_color = 0; // 0 white, 1 gray, 2 black

    object()
    {
        object_type = object_types::number;
        number = 0.0;
    };
    ~object()
    {
        if (object_type == object_types::string)
            string.~basic_string();
        else if (object_type == object_types::function)
            delete func;
    };
};

object *make_object(object_types object_type);