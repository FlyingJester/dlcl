/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#pragma once
#include <cstdlib>

namespace DarkLight {
namespace CL{

class Lexer;
struct Token;

struct Value;

typedef bool (*native_callback)(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);

struct Value {
    enum Type {
        Integer,
        Boolean,
        String,
        Function,
        NativeFunction,
        NumTypes
    } m_type;
    
    union {
        char *string;
        int integer;
        bool boolean;
        unsigned function;
        native_callback native_function;
    } m_value;
    
    union {
        unsigned num_args;
        unsigned length;
        void *arg;
    } a;
    static Type typeFromToken(const Token *t);
    const char *readableTypeName() const;
    
    bool equal(const Value &other) const;
};

struct Variable {
    char m_name[80];
    Value m_value;
};

static const unsigned s_max_parser_arena_size = 0x80 * 0x400;
static const unsigned s_parser_error_len = 0x100;
static const unsigned s_max_parser_variables = 
    (s_max_parser_arena_size / sizeof(struct Variable)) - 8;

class Parser{
    
    unsigned m_sp;
    struct Value m_stack[8];
    struct Value m_arg_stack[16];

    unsigned m_num_variables;
    struct Variable m_variables[s_max_parser_variables];

    unsigned m_callp;
    unsigned m_callstack[8];
    
    char m_error[s_parser_error_len];
    char m_string_data[s_max_parser_arena_size];
    unsigned m_string_length;
    
    int findVariableIndex(const char *name, unsigned len = 0) const;
    
    bool parseStatement(const Token *const start, const Token *&i, const Token *const end);
    bool parseExpression(const Token *const start, const Token *&i, const Token *const end);
    bool parseBTerm(const Token *const start, const Token *&i, const Token *const end);
    bool parseTerm(const Token *const start, const Token *&i, const Token *const end);
    bool parseFactor(const Token *const start, const Token *&i, const Token *const end);
    bool parseAssignment(const Token *const start, const Token *&i, const Token *const end);
    bool parseCall(const Token *const start, const Token *&i, const Token *const end);
    Variable *validateVariableDeclaration(const Token &i);
    
    bool doCall(const char *name, unsigned name_len,
        const Token *const start, const Token *&i, const Token *const end, int /*Operator*/ ender);
    
public:
    
    void bindCallback(const char *name, native_callback, void *arg);
    
    inline Value *findVariable(const char *name, unsigned len = 0) {
        const int r = findVariableIndex(name, len);
        return (r >= 0 && static_cast<unsigned>(r) < s_max_parser_variables) ?
            &(m_variables[r].m_value) : NULL;
    }
    inline const Value *findVariableConst(const char *name, unsigned len = 0) const {
        const int r = findVariableIndex(name, len);
        return (r >= 0 && static_cast<unsigned>(r) < s_max_parser_variables) ? 
            &(m_variables[r].m_value) : NULL;
    }
    inline const Value *findVariable(const char *name, unsigned len = 0) const {
        return findVariableConst(name, len);
    }
    
    Parser();
    
    void clear(){
        m_sp = 0;
        m_num_variables = 0;
        m_string_length = 0;
        m_error[0] = m_error[s_parser_error_len - 1] = '\0';
    }
    
    bool parse(const Lexer &lexer);
    
    const char *getError() const { return m_error; }
};

} // namespace CL
} // namespace DarkLight
