#pragma once
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>

#define MAX_STACK_SIZE 1024
typedef uint64_t bit_64;

typedef enum
{
    gvm_number,
    gvm_string,
    gvm_boolean,
    gvm_function,
    gvm_closure
} gvm_value_type;

typedef struct gvm_value gvm_value;
typedef struct gvm_closure_object gvm_closure_object;

struct gvm_closure_object
{
    gvm_closure_object *parent;

    gvm_value *up_values[256];
    bit_64 up_values_count;

    gvm_value *stack_frame[256];
    bit_64 stack_frame_count;
};

typedef struct
{
    bit_64 start_address;
    bit_64 end_address;
    bit_64 id;
    gvm_closure_object *declaration_parent;

    bit_64 params;
} gvm_function_object;

struct gvm_value
{
    gvm_value_type type;

    union
    {
        double number;
        char *string;
        bool boolean;
        gvm_function_object *function;
        gvm_closure_object *closure;
    };

    uint8_t color; // 0 = White, 1 = Grey, 2 = Black;
};

typedef struct
{
    gvm_value *stack[MAX_STACK_SIZE];
    bit_64 stack_size;
    bit_64 c_address;
    bit_64 program_size;

    gvm_closure_object* closure_history[256];
    bit_64 closure_history_size;

    bool halt;
} GVM;

#define GC_TRIGGER_LIMIT 100
static uint64_t allocations = 0;

static uint64_t gvm_heap_size = 0;
static uint64_t gvm_heap_capacity = 100;
static gvm_value **gvm_heap = NULL;
static void gvm_garbage_collect(GVM *vm);

static inline void gvm_heap_boot()
{
    gvm_heap = malloc(sizeof(gvm_value *) * gvm_heap_capacity);
}

static inline void gvm_free_value(gvm_value *value)
{
    switch (value->type)
    {
    case gvm_string:
        free(value->string);
        break;
    case gvm_function:
        free(value->function);
        break;
    case gvm_closure:
        free(value->closure);
        break;
    default:
        break;
    };

    free(value);
}

static inline gvm_value *gvm_create_value(gvm_value_type type, GVM *vm)
{
    gvm_value *value = malloc(sizeof(gvm_value));
    value->type = type;
    value->color = 0;

    if (gvm_heap_size >= gvm_heap_capacity)
    {
        gvm_heap_capacity *= 2;
        gvm_heap = realloc(gvm_heap, sizeof(gvm_value *) * gvm_heap_capacity);
    }

    gvm_heap[gvm_heap_size] = value;
    gvm_heap_size++;
    allocations++;

    if (allocations > GC_TRIGGER_LIMIT)
    {
        allocations = 0;
        gvm_garbage_collect(vm);
    }

    return value;
}

static inline void gvm_mark_value(gvm_value *value)
{
    if (!value || value->color != 2)
        return;

    value->color = 1;
    value->color = 2;
}

static inline void gvm_garbage_collect(GVM *vm)
{
    for (size_t address = 0; address < vm->stack_size; ++address)
    {
        gvm_mark_value(vm->stack[address]);
    };

    for (size_t address = 0; address < gvm_heap_size; ++address)
    {
        if (gvm_heap[address]->color == 0)
        {
            gvm_free_value(gvm_heap[address]);
            gvm_heap[address] = NULL;
        }
    }
}

static inline void gvm_store_on_stack_frame(gvm_closure_object *closure, gvm_value *value)
{
    closure->stack_frame[closure->stack_frame_count] = value;
    closure->stack_frame_count++;
}
