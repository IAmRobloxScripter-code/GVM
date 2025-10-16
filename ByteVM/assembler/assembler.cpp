#include "assembler.hpp"
#include <cctype>
#include "unordered_map"
#include <iostream>
#include "../iobytes.hpp"
#include "../instructions.hpp"
#include "../object.h"
#include <sstream>
#include <fstream>
#include <string>
#include "../header.h"
#include <cstdlib>

gvm_assembly_token make_token(const std::string &value, gvm_assembly_types gvma_type)
{
    gvm_assembly_token token;
    token.gvmat_type = gvma_type;
    token.value = value;
    return token;
}

bool is_alpha(const std::string &c)
{
    return std::isalpha(c[0]) || std::isdigit(c[0]) || c == "_";
}

std::unordered_map<std::string, gvm_assembly_types> defined{
    {"push", gvm_assembly_types::gvma_push},
    {"add", gvm_assembly_types::gvma_add},
    {"store_local", gvm_assembly_types::gvma_store},
    {"load_local", gvm_assembly_types::gvma_load},
    {"ret", gvm_assembly_types::gvma_ret},
    {"call", gvm_assembly_types::gvma_call}};

std::deque<std::string> split(const std::string &source)
{
    std::deque<std::string> chars;

    for (auto c : source)
    {
        chars.push_back(std::string(1, c));
    }

    return chars;
}

std::string shift(std::deque<std::string> &chars)
{
    std::string c = chars.front();
    chars.pop_front();
    return c;
}

std::deque<gvm_assembly_token> tokenize(const std::string &gvm_assembly)
{
    std::deque<gvm_assembly_token> tokens;
    std::deque<std::string> chars = split(gvm_assembly);

    while (!chars.empty())
    {
        std::string c = chars.front();

        if (std::isspace(c[0]))
        {
            shift(chars);
            continue;
        };

        if (c == ":")
        {
            tokens.push_back(make_token(shift(chars), gvm_assembly_types::gvma_colon));
        }
        else if (is_alpha(c))
        {
            std::string word;

            while (!chars.empty() && is_alpha(chars[0]))
            {
                word += shift(chars);
            }

            if (defined.find(word) != defined.end())
            {
                tokens.push_back(make_token(word, defined[word]));
            }
            else
            {
                tokens.push_back(make_token(word, gvm_assembly_types::gvma_data));
            }
        }
    }
    tokens.push_back(make_token("nop", gvm_assembly_types::gvma_eof));

    return tokens;
}

gvm_assembly_token parser::pop()
{
    gvm_assembly_token token = parser::tokens.front();
    parser::tokens.pop_front();
    return token;
}

std::unique_ptr<gvma_ast_token> parser::parse()
{
    gvm_assembly_token token = parser::pop();

    switch (token.gvmat_type)
    {
    case gvm_assembly_types::gvma_push:
    {
        std::unique_ptr<gvma_ast_push> instruction_push = std::make_unique<gvma_ast_push>();
        instruction_push->__type = parser::pop().value;
        instruction_push->__value = parser::pop().value;
        instruction_push->kind = gvma_ast_kinds::gvma_push;
        return instruction_push;
    }
    case gvm_assembly_types::gvma_add:
    {
        std::unique_ptr<gvma_ast_add> instruction_add = std::make_unique<gvma_ast_add>();
        instruction_add->kind = gvma_ast_kinds::gvma_add;
        return instruction_add;
    };
    case gvm_assembly_types::gvma_store:
    {
        std::unique_ptr<gvma_ast_store> instruction_store = std::make_unique<gvma_ast_store>();
        instruction_store->kind = gvma_ast_kinds::gvma_store;
        instruction_store->__value = parser::pop().value;
        return instruction_store;
    }
    case gvm_assembly_types::gvma_load:
    {
        std::unique_ptr<gvma_ast_load> instruction_store = std::make_unique<gvma_ast_load>();
        instruction_store->kind = gvma_ast_kinds::gvma_load;
        instruction_store->__value = parser::pop().value;
        return instruction_store;
    }
    case gvm_assembly_types::gvma_data:
    {
        if (parser::tokens.front().gvmat_type == gvm_assembly_types::gvma_colon)
        {
            // label parsing or function, same thing.

            parser::pop();

            std::unique_ptr<gvma_ast_label> instruction_label = std::make_unique<gvma_ast_label>();
            instruction_label->kind = gvma_ast_kinds::gvma_label;
            instruction_label->__name = token.value;

            while (!parser::tokens.empty() && parser::tokens.front().value != "_endupv")
            {
                instruction_label->__upvalues.push_back(parser::pop().value);
            }

            parser::pop();
            instruction_label->arguments = std::stoi(parser::pop().value);

            while (!parser::tokens.empty() && parser::tokens.front().value != "endfunc")
            {
                instruction_label->__chunk.push_back(parser::parse());
            }

            parser::pop();

            return instruction_label;
        }

        return nullptr;
    }
    case gvm_assembly_types::gvma_ret:
    {
        std::unique_ptr<gvma_ast_token> instruction_ret = std::make_unique<gvma_ast_token>();
        instruction_ret->kind = gvma_ast_kinds::gvma_ret;
        return instruction_ret;
    }
    case gvm_assembly_types::gvma_call:
    {
        std::unique_ptr<gvma_ast_token> instruction_ret = std::make_unique<gvma_ast_token>();
        instruction_ret->kind = gvma_ast_kinds::gvma_call;
        return instruction_ret;
    }
    default:
        return nullptr;
    }
}

uint8_t get_type_from_string(std::string &__type)
{
    if (__type == "number")
        return static_cast<uint8_t>(object_types::number);
    else if (__type == "string")
        return static_cast<uint8_t>(object_types::string);
    else if (__type == "boolean")
        return static_cast<uint8_t>(object_types::boolean);
    else
        return 0;
}

void gvma_compile(program *output, std::vector<uint8_t> &stream, std::deque<std::unique_ptr<gvma_ast_token>> &ast)
{
    for (std::unique_ptr<gvma_ast_token> &tk : ast)
    {
        switch (tk->kind)
        {
        case gvma_ast_kinds::gvma_push:
        {
            auto node = static_cast<gvma_ast_push *>(tk.get());
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_push));

            uint8_t value_type = get_type_from_string(node->__type);

            write_bytes<uint8_t>(stream, value_type);

            switch (static_cast<object_types>(value_type))
            {
            case object_types::number:
                write_bytes<double>(stream, std::stod(node->__value));
                break;
            case object_types::string:
            {
                for (size_t i = 0; i < node->__value.size(); ++i)
                {
                    write_bytes<char>(stream, node->__value[i]);
                }
                write_bytes<char>(stream, 0);
                break;
            }
            case object_types::boolean:
                write_bytes<bool>(stream, node->__value == "false" ? 0 : 1);
                break;
            default:
                break;
            }
            break;
        }
        case gvma_ast_kinds::gvma_add:
        {
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_add));
            break;
        }
        case gvma_ast_kinds::gvma_store:
        {
            auto node = static_cast<gvma_ast_store *>(tk.get());
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_store));
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(std::stoi(node->__value)));
            break;
        }
        case gvma_ast_kinds::gvma_load:
        {
            auto node = static_cast<gvma_ast_load *>(tk.get());
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_load));
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(std::stoi(node->__value)));
            break;
        }
        case gvma_ast_kinds::gvma_label:
        {
            auto node = static_cast<gvma_ast_label *>(tk.get());

            /*
            le prototype or sum

            name
            param count
            upvalues
            size

            code

            */

            std::vector<uint8_t> chunk;
            write_bytes<uint8_t>(chunk, static_cast<uint8_t>(instructions::gvm_label));

            for (char character : node->__name)
            {
                write_bytes<char>(chunk, character);
            }

            write_bytes<char>(chunk, 0);
            write_bytes<uint8_t>(chunk, node->arguments);

            write_bytes<uint8_t>(chunk, node->__upvalues.size());
            for (size_t upvalue = 0; upvalue < node->__upvalues.size(); upvalue++)
            {
                write_bytes<uint8_t>(chunk, (uint8_t)std::stoi(node->__upvalues[upvalue]));
            }

            std::vector<uint8_t> code;

            gvma_compile(output, code, node->__chunk);
            write_bytes<uint64_t>(chunk, chunk.size() + code.size() + sizeof(uint64_t));

            chunk.insert(
                chunk.end(),
                code.begin(),
                code.end());

            output->_prototypes.insert(
                output->_prototypes.end(),
                chunk.begin(),
                chunk.end());

            // create
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_function));
            for (auto character : node->__name)
            {
                write_bytes<char>(stream, character);
            }
            write_bytes<char>(stream, 0);
            break;
        }
        case gvma_ast_kinds::gvma_ret:
        {
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_ret));
            break;
        }
        case gvma_ast_kinds::gvma_call:
        {
            write_bytes<uint8_t>(stream, static_cast<uint8_t>(instructions::gvm_call));
            break;
        }
        default:
            std::cerr << "Invalid ast!\n";
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <inputfile> [-o <outputfile>]\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path;

    if (argc >= 4 && std::string(argv[2]) == "-o")
    {
        output_path = argv[3];
    }
    else
    {
        output_path = input_path + ".o";
    }

    std::ifstream input(input_path);
    if (!input.is_open())
    {
        std::cerr << "Failed to open input file: " << input_path << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();

    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open())
    {
        std::cerr << "Failed to open output file: " << output_path << "\n";
        return 1;
    }

    auto tokens = tokenize(content);
    parser *parser_class = new parser;
    parser_class->tokens = tokens;

    while (!tokens.empty())
    {
        auto value = parser_class->parse();
        if (value == nullptr)
        {
            break;
        }
        parser_class->ast.push_back(std::move(value));
    }

    program *__program = new program;

    header head;
    write_bytes<header>(__program->_header, head);
    gvma_compile(__program, __program->_main, parser_class->ast);

    auto write_vec = [&](const std::vector<uint8_t> &buf)
    {
        if (!buf.empty())
            output.write(reinterpret_cast<const char *>(buf.data()), buf.size());
    };

    write_vec(__program->_header);
    write_vec(__program->_prototypes);
    write_vec(__program->_main);

    delete parser_class;
    delete __program;

    return 0;
}
