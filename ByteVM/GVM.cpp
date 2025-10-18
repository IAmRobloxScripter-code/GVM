#include "instructions.hpp"
#include "iobytes.hpp"
#include <vector>
#include "object.h"
#include <string>
#include <sstream>
#include "header.h"
#include <unordered_map>

struct label_proto
{
    std::vector<uint8_t> upvalues;
    uint64_t ip;
    uint64_t program_size;
    uint8_t param_count;
    uint64_t header_size;
};

class GVM
{
public:
    std::vector<object *> heap_objects;
    std::vector<stack_frame *> heap_stack_frames;
    double memory_allocated = 0;

    std::vector<object *> stack;
    std::vector<uint64_t> return_position_pointers;
    std::vector<stack_frame *> stack_frames;
    std::unordered_map<std::string, label_proto *> labels;
    uint64_t ip = 0;

public:
    object *stack_pop()
    {
        object *value = stack.back();
        stack.pop_back();
        return value;
    }

    stack_frame *get_running_stack_frame()
    {
        return stack_frames[stack_frames.size() - 1];
    }

    void dump_stack()
    {
        if (stack.empty())
        {
            std::cout << "Stack: \n [empty]" << std::endl;
            return;
        };
        std::cout << "Stack: \n";
        for (object *obj : stack)
        {
            switch (obj->object_type)
            {
            case object_types::number:
                std::cout << "  " << obj->number << std::endl;
                break;
            case object_types::string:
                std::cout << "  " << obj->string << std::endl;
                break;
            case object_types::boolean:
                std::cout << "  " << (obj->boolean == 0 ? "false" : "true") << std::endl;
                break;
            case object_types::function:
                std::cout << "  " << obj->func << std::endl;
                break;
            default:
                break;
            };
        }
    }
};

struct gc_data
{
    std::vector<object *> gray_objects;
    std::vector<stack_frame *> gray_stack_frames;
};

void mark_gc_object(gc_data *gc, object *gc_object)
{
    gc_object->gc_color = 2;

    if (gc_object->object_type == object_types::function && gc_object->func)
    {
        for (object *upvalue : gc_object->func->upvalues)
        {
            gc_object->gc_color = 1;
            gc->gray_objects.push_back(upvalue);
        }
    }
}

void mark_gc_stack_frame(gc_data *gc, stack_frame *gc_object)
{
    gc_object->gc_color = 2;

    for (object *local : gc_object->locals)
    {
        gc_object->gc_color = 1;
        gc->gray_objects.push_back(local);
    }
}

void init_garbage_collection(GVM *vm)
{
    gc_data *gc = new gc_data;

    for (object *gc_object : vm->heap_objects)
    {
        gc_object->gc_color = 0;
    }

    for (stack_frame *gc_object : vm->heap_stack_frames)
    {
        gc_object->gc_color = 0;
    }

    for (object *gc_object : vm->stack)
    {
        gc_object->gc_color = 1;
        gc->gray_objects.push_back(gc_object);
    }

    for (stack_frame *gc_object : vm->stack_frames)
    {
        gc_object->gc_color = 1;
        gc->gray_stack_frames.push_back(gc_object);
    }

    while (gc->gray_objects.size() > 0)
    {
        object *gc_object = gc->gray_objects.back();
        gc->gray_objects.pop_back();

        mark_gc_object(gc, gc_object);
    }

    while (gc->gray_stack_frames.size() > 0)
    {
        stack_frame *gc_object = gc->gray_stack_frames.back();
        gc->gray_stack_frames.pop_back();

        mark_gc_stack_frame(gc, gc_object);
    }

    for (object *gc_object : vm->heap_objects)
    {
        if (gc_object->gc_color == 0)
        {
            delete gc_object;
        }
    }

    for (stack_frame *gc_object : vm->stack_frames)
    {
        if (gc_object->gc_color == 0)
        {
            delete gc_object;
        }
    }

    delete gc;
}

double get_size_in_mb(uint64_t bytes)
{
    return bytes / (1024.0 * 1024.0);
}

void add_mem_and_check_for_gc(GVM *vm, double size)
{
    vm->memory_allocated += get_size_in_mb(size);

    if (vm->memory_allocated > GC_TRIGGER_MEMORY_COUNT_MB)
    {
        vm->memory_allocated = 0;
        init_garbage_collection(vm);
    }
}

object *make_object(object_types object_type)
{
    object *object_value = new object{};
    object_value->object_type = object_type;
    object_value->gc_color = 2;

    if (object_type == object_types::string)
        new (&object_value->string) std::string("");

    return object_value;
}

template <typename T>
T bytes_to_number(uint8_t *bytes)
{
    T n;
    std::memcpy(&n, bytes, sizeof(T));
    return n;
}

int main(int argc, char *argv[])
{
    (void)argc;
    std::ifstream byte_file(argv[1], std::ios::binary | std::ios::ate);

    size_t filesize = byte_file.tellg();
    byte_file.seekg(0, std::ios::beg);

    header header_compiled;
    header expected_header;

    byte_file.read(reinterpret_cast<char *>(&header_compiled), sizeof(header));

    if (!byte_file)
    {
        std::cerr << "Failed to read header!\n";
        return 1;
    }

    if (header_compiled.identifier != expected_header.identifier)
    {
        std::cerr << "Invalid bytecode!\n";
        return 1;
    };

    if (header_compiled.version_major != expected_header.version_major || header_compiled.version_minor != expected_header.version_minor)
    {
        std::cerr << "Unsupported version!\n";
        return 1;
    }

    uint8_t remaining_size = filesize - sizeof(header);
    uint8_t *bytes = new uint8_t[remaining_size];

    byte_file.read((char *)bytes, remaining_size);
    byte_file.close();

    GVM vm;

    stack_frame *frame = new stack_frame;
    vm.stack_frames.push_back(frame);

    vm.dump_stack();
    while (vm.ip < remaining_size)
    {
        // std::cout << static_cast<int>(bytes[vm.ip]) << std::endl;
        switch (static_cast<instructions>(bytes[vm.ip]))
        {
        case instructions::gvm_push:
        {
            vm.ip++;

            uint8_t type_of_value = bytes_to_number<uint8_t>(&bytes[vm.ip]);
            vm.ip++;

            switch (static_cast<object_types>(type_of_value))
            {
            case object_types::number:
            {
                double value = bytes_to_number<double>(&bytes[vm.ip]);
                vm.ip += 8;

                object *object_value = make_object(object_types::number);
                object_value->number = value;

                vm.stack.push_back(object_value);
                vm.heap_objects.push_back(object_value);
                add_mem_and_check_for_gc(&vm, sizeof(*object_value));
                break;
            }
            case object_types::string:
            {
                std::string value;

                while (vm.ip < filesize && bytes[vm.ip] != 0)
                {
                    value += (char)bytes[vm.ip];
                    vm.ip++;
                }
                vm.ip++;

                object *object_value = make_object(object_types::string);
                object_value->string = value;

                vm.stack.push_back(object_value);
                vm.heap_objects.push_back(object_value);
                add_mem_and_check_for_gc(&vm, sizeof(*object_value));
                break;
            }

            case object_types::boolean:
            {
                bool value = bytes_to_number<bool>(&bytes[vm.ip]);
                vm.ip++;

                object *object_value = make_object(object_types::boolean);
                object_value->boolean = value;

                vm.stack.push_back(object_value);
                vm.heap_objects.push_back(object_value);
                add_mem_and_check_for_gc(&vm, sizeof(*object_value));
                break;
            }
            default:
                break;
            }

            break;
        }
        case instructions::gvm_add:
        {
            object *y = vm.stack_pop();
            object *x = vm.stack_pop();

            double result = x->number + y->number;

            object *object_value = make_object(object_types::number);
            object_value->number = result;

            vm.stack.push_back(object_value);
            vm.heap_objects.push_back(object_value);
            add_mem_and_check_for_gc(&vm, sizeof(*object_value));

            vm.ip++;
            break;
        }
        case instructions::gvm_store:
        {
            vm.ip++;
            object *value = vm.stack_pop();
            uint8_t id = bytes_to_number<uint8_t>(&bytes[vm.ip]);
            vm.ip++;

            stack_frame *frame = vm.get_running_stack_frame();
            if (id >= frame->locals.size())
            {
                frame->locals.resize(id + 1);
            }
            frame->locals[id] = value;
            break;
        }
        case instructions::gvm_load:
        {
            vm.ip++;
            uint8_t id = bytes_to_number<uint8_t>(&bytes[vm.ip]);
            vm.ip++;

            stack_frame *frame = vm.get_running_stack_frame();
            vm.stack.push_back(frame->locals[id]);
            break;
        }
        case instructions::gvm_label:
        {
            vm.ip++;

            /*
            le prototype or sum

            name
            param count
            upvalues
            size

            code

            */

            std::string name;
            label_proto *label = new label_proto;
            label->ip = vm.ip;

            while (vm.ip < filesize && bytes[vm.ip] != 0)
            {
                name += (char)bytes[vm.ip];
                vm.ip++;
                label->ip++;
            }
            vm.ip++;
            label->ip++;

            label->param_count = (int)bytes_to_number<uint8_t>(&bytes[vm.ip]);
            vm.ip++;
            label->ip++;

            uint8_t upcount = bytes_to_number<uint8_t>(&bytes[vm.ip]);
            vm.ip++;
            label->ip++;

            for (uint8_t index = 0; index < upcount; ++index)
            {
                label->upvalues.push_back(bytes_to_number<uint8_t>(&bytes[vm.ip]));
                vm.ip++;
                label->ip++;
            }

            uint64_t size = bytes_to_number<uint64_t>(&bytes[vm.ip]);
            vm.ip += 8;
            label->ip += 8;

            label->program_size = size;
            // label->header_size = (vm.ip - label->ip) + 2;

            vm.labels[name] = label;
            vm.ip = vm.ip + size;

            break;
        }
        case instructions::gvm_function:
        {
            vm.ip++;
            std::string name;
            while (vm.ip < filesize && bytes[vm.ip] != 0)
            {
                name += (char)bytes[vm.ip];
                vm.ip++;
            };
            vm.ip++;

            function *proto = new function;
            proto->ip = vm.labels[name]->ip;
            proto->params = vm.labels[name]->param_count;
            proto->name = name;
            // proto->header_size = vm.labels[name]->header_size;

            for (uint8_t upvalue : vm.labels[name]->upvalues)
            {
                proto->upvalues.push_back(vm.get_running_stack_frame()->locals[upvalue]);
            }

            object *object_value = make_object(object_types::function);
            object_value->func = proto;

            vm.stack.push_back(object_value);
            vm.heap_objects.push_back(object_value);

            add_mem_and_check_for_gc(&vm, sizeof(*object_value));
            break;
        }
        case instructions::gvm_ret:
        {
            vm.ip++;

            vm.stack_frames.pop_back();
            vm.ip = vm.return_position_pointers.back();
            vm.return_position_pointers.pop_back();

            break;
        }
        case instructions::gvm_call:
        {
            vm.ip++;
            vm.return_position_pointers.push_back(vm.ip);

            object *func = vm.stack_pop();
            stack_frame *frame = new stack_frame;
            frame->last = vm.get_running_stack_frame();

            vm.stack_frames.push_back(frame);
            vm.heap_stack_frames.push_back(frame);
            add_mem_and_check_for_gc(&vm, sizeof(*frame));

            for (object *upvalue : func->func->upvalues)
            {
                frame->locals.push_back(upvalue);
            }

            for (uint8_t argcount = 0; argcount < func->func->params; ++argcount)
            {
                frame->locals.push_back(vm.stack_pop());
            };
            vm.ip = func->func->ip;

            break;
        }
        default:
            std::cerr << "Invalid instruction! " << static_cast<uint8_t>(bytes[vm.ip]) << "\n";
            exit(1);
        }
    }
    vm.dump_stack();
}