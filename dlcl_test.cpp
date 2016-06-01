/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_lexer.hpp"
#include "dlcl_parser.hpp"
#include "dlcl_std.hpp"
#include "UnitTestPP.h"
#include "TestReporterStdout.h"
#include <cstring>
#include <cstdio>
#include <string>

using namespace DarkLight::CL;

class LexerTest {
public:
    
    bool run(const char *src){
        m_lex.lex(src, strlen(src));
        const bool r = m_lex.getError()[0] == '\0';
        if(!r){
            fputs("Lexer Error: ", stdout);
            puts(m_lex.getError());
        }
        return r;
    }
    
    const Lexer &lexer() const { return m_lex; }
    
protected:
    Lexer m_lex;
    
    bool empty() const { return m_lex.cbegin() == m_lex.cend(); }
    unsigned size() const { return m_lex.cend() - m_lex.cbegin(); }
    
};

SUITE(Lexer){

// Sanity check on empty input
TEST_FIXTURE(LexerTest, EmptyTest){
    static const unsigned num_tests = 6;
    const char *tests[num_tests] = {
        "",
        "  ",
        " \t ",
        "\n\n",
        "  \t\n ",
        "\r",
    };

    for(unsigned i = 0; i < num_tests; i++){
        REQUIRE CHECK(run(tests[i]));
        CHECK(empty());
    }
}

TEST_FIXTURE(LexerTest, CommentTest){
    REQUIRE CHECK(run(" % This is a comment! "));
    CHECK(empty());
    
    m_lex.clear();
    
    REQUIRE CHECK(run(" % this is a comment! \n string other \"but this is not!\""));
    CHECK(!empty());
    CHECK(m_lex.cbegin()->m_type == Token::StringIdent);
}

TEST_FIXTURE(LexerTest, CallTest){
    REQUIRE CHECK(run("call somefunc."));
    CHECK_EQUAL(2, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 2);
    CHECK_EQUAL(Token::CallIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somefunc", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
    CHECK_EQUAL(Token::Dot, m_lex.cbegin()[1].m_type);
}

TEST_FIXTURE(LexerTest, SetTest){
    REQUIRE CHECK(run("set somevar 10"));
    CHECK_EQUAL(2, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 2);
    CHECK_EQUAL(Token::SetIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somevar", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
    CHECK_EQUAL(Token::Number, m_lex.cbegin()[1].m_type);
    CHECK_EQUAL(10, m_lex.cbegin()[1].m_value.number);
}

TEST_FIXTURE(LexerTest, GetTest){
    REQUIRE CHECK(run("get somevar"));
    CHECK_EQUAL(1, m_lex.size());
    REQUIRE CHECK(!m_lex.empty());
    CHECK_EQUAL(Token::GetIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somevar", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
}

TEST_FIXTURE(LexerTest, MulOpTest){
    REQUIRE CHECK(run("int i 10 * 2 / 299 * 1"));
    CHECK_EQUAL(8, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 8);
    
    const Token *t = m_lex.cbegin();
    CHECK_EQUAL(Token::IntIdent, t->m_type);
    CHECK_EQUAL("i", std::string(t->m_value.string, t->m_length));
    t++;
    CHECK_EQUAL(Token::Number, t->m_type);
    CHECK_EQUAL(10, t->m_value.number);
    t++;
    CHECK_EQUAL(Token::Oper, t->m_type);
    CHECK_EQUAL(Multiply, t->m_value.oper);
    t++;
    CHECK_EQUAL(Token::Number, t->m_type);
    CHECK_EQUAL(2, t->m_value.number);
    t++;
    CHECK_EQUAL(Token::Oper, t->m_type);
    CHECK_EQUAL(Divide, t->m_value.oper);
    t++;
    CHECK_EQUAL(Token::Number, t->m_type);
    CHECK_EQUAL(299, t->m_value.number);
    t++;
    CHECK_EQUAL(Token::Oper, t->m_type);
    CHECK_EQUAL(Multiply, t->m_value.oper);
    t++;
    CHECK_EQUAL(Token::Number, t->m_type);
    CHECK_EQUAL(1, t->m_value.number);
}

TEST_FIXTURE(LexerTest, AddOpTest){
    REQUIRE CHECK(run("string \nstr \n\"abc\" \n+\n \"xyz\""));
    CHECK_EQUAL(4, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 4);
    
    const Token *t = m_lex.cbegin();
    CHECK_EQUAL(Token::StringIdent, t->m_type);
    CHECK_EQUAL("str", std::string(t->m_value.string, t->m_length));
    t++;
    CHECK_EQUAL(Token::String, t->m_type);
    CHECK_EQUAL("abc", std::string(t->m_value.string, t->m_length));
    t++;
    CHECK_EQUAL(Token::Oper, t->m_type);
    CHECK_EQUAL(Plus, t->m_value.oper);
    t++;
    CHECK_EQUAL(Token::String, t->m_type);
    CHECK_EQUAL("xyz", std::string(t->m_value.string, t->m_length));
}

} // SUITE(Lexer)

class ParserTest {
protected:
    LexerTest m_lex;
    Parser m_parse;
    
    bool run(const char *src){
        if(!m_lex.run(src))
            return false;
        m_parse.parse(m_lex.lexer());

        const bool r = m_parse.getError()[0] == '\0';
        if(!r){
            fputs("Parser Error: ", stdout);
            puts(m_parse.getError());
        }
        return r;
    }

    template<int N>
    struct Result { bool m_success; Value m_values[N]; };

    static bool TestNoArgsCallback(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
        *static_cast<bool *>(arg) = num_args == 0;
        return true;
    }
    
    template<int N>
    static bool TestArgCallback(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
        struct Result<N> *const result = static_cast<struct Result<N> *>(arg);
        result->m_success = true;
        if(num_args == N){
            for(int i = 0; i < N; i++){
                const bool r = result->m_values[i].equal(args[i]);
                result->m_success = result->m_success && r;
            }
        }
        return true;
    }
};

SUITE(Parser){

TEST_FIXTURE(ParserTest, BindNativeTest){
    const char *const testname = "TestFunc";
    m_parse.bindCallback(testname, TestNoArgsCallback, this);
    
    const Value *function = m_parse.findVariableConst(testname);
    
    REQUIRE CHECK(function != NULL);
    REQUIRE CHECK_EQUAL(Value::NativeFunction, function->m_type);
    CHECK_EQUAL(TestNoArgsCallback, function->m_value.native_function);
    CHECK_EQUAL((void *)this, function->a.arg);
}

TEST_FIXTURE(ParserTest, NoArgsNativeCallback){
    const char *const testname = "TestFunc";
    bool success;
    m_parse.bindCallback(testname, TestNoArgsCallback, &success);
    CHECK(run("call TestFunc."));
    CHECK(success);
}

TEST_FIXTURE(ParserTest, FailNoArgsNativeCallback){
    const char *const testname = "TestFunc";
    bool success;
    m_parse.bindCallback(testname, TestNoArgsCallback, &success);
    CHECK(run("call TestFunc: 1."));
    CHECK(!success);
}

TEST_FIXTURE(ParserTest, SingleArgNativeCallback){
    const char *const testname = "TestFunc";
    Result<1> result;
    result.m_success = false;

    result.m_values[0].m_type = Value::Integer;
    result.m_values[0].m_value.integer = 1;

    m_parse.bindCallback(testname, TestArgCallback<1>, &result);
    CHECK(run("call TestFunc: 1."));
    CHECK(result.m_success);
}

TEST_FIXTURE(ParserTest, ThreeArgsNativeCallback){
    const char *const testname = "TestFunc",
        *const str = "It's a TRAP!";
    char string_buffer[80];
    const unsigned len = strlen(str);
    REQUIRE CHECK(len < sizeof(string_buffer));
    
    memcpy(string_buffer, str, len);
    
    Result<3> result;
    result.m_success = false;

    result.m_values[0].m_type = Value::Integer;
    result.m_values[0].m_value.integer = 1;
    
    result.m_values[1].m_type = Value::String;
    result.m_values[1].m_value.string = string_buffer;
    result.m_values[1].a.length =  len;
    
    result.m_values[2].m_type = Value::Boolean;
    result.m_values[2].m_value.boolean = true;
    
    m_parse.bindCallback(testname, TestArgCallback<3>, &result);
    CHECK(run("call TestFunc: 1 \"It's a TRAP!\" true."));
    CHECK(result.m_success);
}

TEST_FIXTURE(ParserTest, SmallTalkCallTest){
    const char *const testname = "TestFunc";
    Result<2> result;
    result.m_success = false;

    result.m_values[0].m_type = Value::Integer;
    result.m_values[0].m_value.integer = 23;

    result.m_values[1].m_type = Value::Boolean;
    result.m_values[1].m_value.boolean = false;

    m_parse.bindCallback(testname, TestArgCallback<2>, &result);
    CHECK(run("[TestFunc 23 false]"));
    CHECK(result.m_success);
}

TEST_FIXTURE(ParserTest, StringAddTest){
    const char *const testname = "TestFunc",
        *const str = "What a twist?";
    char string_buffer[80];
    const unsigned len = strlen(str);
    REQUIRE CHECK(len < sizeof(string_buffer));

    memcpy(string_buffer, str, len);

    Result<1> result;
    result.m_success = false;

    result.m_values[0].m_type = Value::String;
    result.m_values[0].m_value.string = string_buffer;
    result.m_values[0].a.length = len;
    
    m_parse.bindCallback(testname, TestArgCallback<1>, &result);
    CHECK(run("call TestFunc: \"What a \" + \"twist\" + \"?\"."));
    CHECK(result.m_success);
}

TEST_FIXTURE(ParserTest, FailStringAddTest){
    const char *const testname = "TestFunc",
        *const str = "What a twist?";
    char string_buffer[80];
    const unsigned len = strlen(str);
    REQUIRE CHECK(len < sizeof(string_buffer));

    memcpy(string_buffer, str, len);

    Result<1> result;
    result.m_success = true;

    result.m_values[0].m_type = Value::String;
    result.m_values[0].m_value.string = string_buffer;
    result.m_values[0].a.length = len;
    
    m_parse.bindCallback(testname, TestArgCallback<1>, &result);
    CHECK(run("call TestFunc: \"What a \" + \"joke\" + \"?\"."));
    CHECK(!result.m_success);
}

TEST_FIXTURE(ParserTest, NumberArithmeticTest){
    const char *const testname = "TestFunc";
    Result<1> result;
    result.m_success = false;

    result.m_values[0].m_type = Value::Integer;
    result.m_values[0].m_value.integer = 216;
    
    m_parse.bindCallback(testname, TestArgCallback<1>, &result);
    CHECK(run("call TestFunc: 200 + ( 17 - 1)."));
    CHECK(result.m_success);
}

} // SUITE(Parser)

int main(int argc, char *argv[]){
    
	UnitTest::TestReporterStdout reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}