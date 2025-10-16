#pragma once
#include "GVM.h"

static void gvm_add_operator(GVM* vm) {
    vm->stack[vm->stack_size - 2]->number += vm->stack[vm->stack_size - 1]->number;
    vm->stack_size--;
}