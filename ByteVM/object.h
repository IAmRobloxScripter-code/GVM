#pragma once
#include <string>

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