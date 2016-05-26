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
protected:
    Lexer m_lex;
    const char *m_src;
    
    bool run(){
        m_lex.lex(m_src, strlen(m_src));
        const bool r = m_lex.getError()[0] == '\0';
        if(!r)
            puts(m_lex.getError());
        return r;
    }
    
    bool empty() const { return m_lex.cbegin() == m_lex.cend(); }
    unsigned size() const { return m_lex.cend() - m_lex.cbegin(); }
    
};

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
        m_src = tests[i];
        REQUIRE CHECK(run());
        CHECK(empty());
    }
}

TEST_FIXTURE(LexerTest, CommentTest){
    m_src = " % This is a comment! ";
    REQUIRE CHECK(run());
    CHECK(empty());
    
    m_lex.clear();
    
    m_src = " % this is a comment! \n string other \"but this is not!\"";
    REQUIRE CHECK(run());
    CHECK(!empty());
    CHECK(m_lex.cbegin()->m_type == Token::StringIdent);
}

TEST_FIXTURE(LexerTest, CallTest){
    m_src = "call somefunc.";
    REQUIRE CHECK(run());
    CHECK_EQUAL(2, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 2);
    CHECK_EQUAL(Token::CallIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somefunc", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
    CHECK_EQUAL(Token::Dot, m_lex.cbegin()[1].m_type);
}

TEST_FIXTURE(LexerTest, SetTest){
    m_src = "set somevar 10";
    REQUIRE CHECK(run());
    CHECK_EQUAL(2, m_lex.size());
    REQUIRE CHECK(m_lex.size() >= 2);
    CHECK_EQUAL(Token::SetIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somevar", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
    CHECK_EQUAL(Token::Number, m_lex.cbegin()[1].m_type);
    CHECK_EQUAL(10, m_lex.cbegin()[1].m_value.number);
}

TEST_FIXTURE(LexerTest, GetTest){
    m_src = "get somevar";
    REQUIRE CHECK(run());
    CHECK_EQUAL(1, m_lex.size());
    REQUIRE CHECK(!m_lex.empty());
    CHECK_EQUAL(Token::GetIdent, m_lex.cbegin()[0].m_type);
    CHECK_EQUAL("somevar", std::string(m_lex.cbegin()[0].m_value.string, m_lex.cbegin()[0].m_length));
}

TEST_FIXTURE(LexerTest, MulOpTest){
    m_src = "int i 10 * 2 / 299 * 1";
    REQUIRE CHECK(run());
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
    m_src = "string \nstr \n\"abc\" \n+\n \"xyz\"";
    REQUIRE CHECK(run());
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

int main(int argc, char *argv[]){
    
	UnitTest::TestReporterStdout reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}
