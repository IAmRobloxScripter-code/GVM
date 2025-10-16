#include "GVM_UTILS.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
  Err_OK,
  Err_Stack_Underflow,
  Err_Stack_Overflow,
  Err_Invalid_Instruction
} Error;

char *Error_to_string(Error err)
{
  switch (err)
  {
  case Err_OK:
    return "None";
  case Err_Stack_Underflow:
    return "Stack Underflow";
  case Err_Stack_Overflow:
    return "Stack Overflow";
  case Err_Invalid_Instruction:
    return "Invalid Instruction";
  default:
    return "Invalid error type!";
  }
}

typedef enum
{
  gvm_push,
  gvm_load,
  gvm_add,
  gvm_sub,
  gvm_mul,
  gvm_div,
  gvm_jmp,
  gvm_make,
  gvm_call,
  gvm_ret,
  gvm_eq,
  gvm_neq,
  gvm_gte,
  gvm_lte,
  gvm_gt,
  gvm_lt,
  gvm_not
} op_codes;

typedef enum
{
  _iftrue,
  _iffalse,
  _local,
  _function,
  _native,
} op_codes_mnemonics;

typedef struct
{
  op_codes op_code;
  op_codes_mnemonics mnemonic;
  gvm_value *operand;
  bit_64 operand_as_bit64;
} gem_instruction;

#define GET_PROGRAM_SIZE(program) sizeof((program)) / sizeof((program)[0])
#define MAKE_PUSH_INSTRUCTION(value) \
  (gem_instruction) { .op_code = gvm_push, .operand = (value) }
#define MAKE_ADD_INSTRUCTION \
  (gem_instruction) { .op_code = gvm_add }
#define MAKE_MAKE_INSTRUCTION(op_code_mnemonic, value)                      \
  (gem_instruction)                                                         \
  {                                                                         \
    .op_code = gvm_make, .mnemonic = (op_code_mnemonic), .operand = (value) \
  }
#define MAKE_CALL_INSTRUCTION(value) \
  (gem_instruction) { .op_code = gvm_call, .operand_as_bit64 = (value) }
#define MAKE_RET_INSTRUCTION(value) \
  (gem_instruction) { .op_code = gvm_ret, .operand_as_bit64 = (value) }
#define MAKE_LOAD_INSTRUCTION(value)                 \
  (gem_instruction)                                  \
  {                                                  \
    .op_code = gvm_load, .operand_as_bit64 = (value) \
  }
char *opcode_to_string(op_codes op_code)
{
  switch (op_code)
  {
  case gvm_push:
    return "push";
  case gvm_add:
    return "add";
  case gvm_call:
    return "call";
  case gvm_ret:
    return "ret";
  case gvm_load:
    return "load";
  default:
    return "Invalid opcode";
  }
}

Error execute_instruction(GVM *vm, gvm_closure_object *closure,
                          gem_instruction instruction)
{
  if (vm->c_address >= vm->program_size)
  {
    vm->halt = true;
    return Err_OK;
  }

  switch (instruction.op_code)
  {
  case gvm_push:
    if (vm->stack_size >= MAX_STACK_SIZE)
    {
      return Err_Stack_Overflow;
    }

    vm->stack[vm->stack_size++] = instruction.operand;
    vm->c_address++;

    return Err_OK;
  case gvm_add:
    if (vm->stack_size < 2)
    {
      return Err_Stack_Underflow;
    }

    gvm_add_operator(vm);
    vm->c_address++;

    return Err_OK;
  case gvm_make:
    if (instruction.mnemonic == _function)
    {
      closure->stack_frame[instruction.operand->function->id] =
          instruction.operand;
      closure->stack_frame_count++;
      vm->c_address++;

      uint64_t cmp = instruction.operand->function->end_address -
                     instruction.operand->function->start_address;
      vm->c_address += cmp;
    }
    else if (instruction.mnemonic == _local)
    {
      if (vm->stack_size < 1)
      {
        return Err_Stack_Underflow;
      }

      closure->stack_frame[closure->stack_frame_count + 1] =
          instruction.operand;
      closure->stack_frame_count++;
    }
    vm->c_address++;
    return Err_OK;
  case gvm_call:
  {
    gvm_value *function_def =
        vm->closure_history[vm->closure_history_size]->stack_frame[instruction.operand_as_bit64];

    gvm_value *closure_value = gvm_create_value(gvm_closure, vm);
    gvm_closure_object *closure_object = malloc(sizeof(gvm_closure_object));
    closure_object->parent = function_def->function->declaration_parent;
    closure_value->closure = closure_object;

    for (size_t index = 0; index < function_def->function->params; ++index)
    {
      closure_object->stack_frame[closure_object->stack_frame_count] = vm->stack[vm->stack_size - 1];
      vm->stack_size--;
      closure_object->stack_frame_count++;
    }

    vm->closure_history_size++;
    vm->closure_history[vm->closure_history_size] = closure_object;
    vm->c_address = function_def->function->start_address;

    return Err_OK;
  }
  case gvm_ret:
  {
    vm->closure_history_size--;
    vm->c_address = instruction.operand_as_bit64 + 1;
    return Err_OK;
  }
  case gvm_load:
  {
    if (vm->stack_size >= MAX_STACK_SIZE)
    {
      return Err_Stack_Overflow;
    }

    vm->stack[vm->stack_size + 1] = vm->closure_history[vm->closure_history_size]->stack_frame[instruction.operand_as_bit64];
    vm->stack_size++;
    vm->c_address++;

    return Err_OK;
  } case gvm_jmp: {
    vm->c_address = instruction.operand_as_bit64;
    return Err_OK;
  }
  default:
    return Err_Invalid_Instruction;
  };
}

char *double_to_string(double num)
{
  char *buffer = malloc(64);
  if (!buffer)
    return NULL;
  sprintf(buffer, "%g", num);
  return buffer;
}

char *gvm_value_to_string(gvm_value *value)
{
  switch (value->type)
  {
  case gvm_number:
    return double_to_string(value->number);
  case gvm_string:
    return value->string;
  case gvm_boolean:
    return value->number == 0 ? "false" : "true";
  case gvm_function:
  {
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "function: %p", value->function);
    return buffer;
  }
  case gvm_closure:
  {
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "closure: %p", value->closure);
    return buffer;
  }
  default:
    return "null";
  }
}

void gvm_dump_stack(FILE *stream, GVM *vm)
{
  fprintf(stream, "Stack: \n");
  if (vm->stack_size == 0)
  {
    fprintf(stream, "   [empty]\n");
    return;
  }
  for (size_t index = 0; index < vm->stack_size; ++index)
  {
    fprintf(stream, "   %s\n", gvm_value_to_string(vm->stack[index]));
  };
}

char* gvm_read_file(char* file) {
    FILE *file = fopen(file, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *text = malloc(size + 1);
    if (!text) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    size_t read = fread(text, 1, size, file);
    text[read] = '\0';

    fclose(file);
    return text;
}

gem_instruction** gvm_build_program_from_text(char* gvsm, GVM* vm) {

}

int main(int argc, char* argv[])
{
  char* gvsm = gvm_read_file(argv[1]);
  GVM gvm = {0};
  gvm_heap_boot();

  gvm_closure_object *closure = malloc(sizeof(gvm_closure_object));
  closure->parent = NULL;
  gvm.closure_history[0] = closure;
  
  gem_instruction program[] = gvm_build_program_from_text(gvsm, &gvm);
 /* gvm_value *valueA = gvm_create_value(gvm_function, &gvm);
  gvm_function_object *function = malloc(sizeof(gvm_function_object));
  function->declaration_parent = NULL;
  function->params = 2;
  function->id = 0;
  function->start_address = 1;
  function->end_address = 4;
  valueA->function = function;

  gvm_value *valueB = gvm_create_value(gvm_number, &gvm);
  valueB->number = 100;
  gvm_value *valueC = gvm_create_value(gvm_number, &gvm);
  valueC->number = 200;*/

  gvm.program_size = GET_PROGRAM_SIZE(program);

  while (gvm.halt != true)
  {
    Error result =
        execute_instruction(&gvm, gvm.closure_history[gvm.closure_history_size], program[gvm.c_address]);

    if (result != Err_OK)
    {
      fprintf(stderr, "Error: %s at instruction %s\n", Error_to_string(result),
              opcode_to_string(program[gvm.c_address].op_code));
      gvm_dump_stack(stderr, &gvm);
      exit(1);
    }
  }

  gvm_dump_stack(stdout, &gvm);
}
