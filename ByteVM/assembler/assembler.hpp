#pragma once

#include <string>
#include <cstdint>
#include <deque>
#include <memory>
#include <fstream>
#include <vector>

enum class gvm_assembly_types : u_int8_t {
    gvma_push,
    gvma_add,
    gvma_store,
    gvma_data,
    gvma_colon,
    gvma_load,
    gvma_ret,
    gvma_call,
    gvma_eof
};

struct gvm_assembly_token {
    gvm_assembly_types gvmat_type;
    std::string value;
};

std::deque<gvm_assembly_token> tokenize(const std::string &gvm_assembly);

enum class gvma_ast_kinds : u_int8_t {
    gvma_nop,
    gvma_push,
    gvma_add,
    gvma_store,
    gvma_load,
    gvma_label,
    gvma_call,
    gvma_ret
};

struct gvma_ast_token{
    gvma_ast_kinds kind = gvma_ast_kinds::gvma_nop;
    virtual ~gvma_ast_token() = default;
};

struct gvma_ast_push : public gvma_ast_token{
    std::string __type;
    std::string __value;
};

struct gvma_ast_add : public gvma_ast_token{};
struct gvma_ast_store : public gvma_ast_token{
    std::string __value;
};

struct gvma_ast_load: public gvma_ast_token{
    std::string __value;
};

struct gvma_ast_label: public gvma_ast_token{
    std::deque<std::unique_ptr<gvma_ast_token>> __chunk;
    std::string __name;
    uint8_t arguments;
    std::vector<std::string> __upvalues;
};

class parser {
    public:
        std::deque<std::unique_ptr<gvma_ast_token>> ast;
        std::deque<gvm_assembly_token> tokens;

    public:
        gvm_assembly_token pop();
        std::unique_ptr<gvma_ast_token> parse();
};
