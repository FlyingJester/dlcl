#include "dlcl_lexer.hpp"
#include "dlcl_parser.hpp"
#include "UnitTestPP.h"
#include "TestReporterStdout.h"
#include <cstring>
#include <cstdio>

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
    
}

TEST_FIXTURE(LexerTest, SetTest){
    
}

TEST_FIXTURE(LexerTest, GetTest){
    
}

TEST_FIXTURE(LexerTest, MulOpTest){
    
}

TEST_FIXTURE(LexerTest, AddOpTest){
    
}

TEST_FIXTURE(LexerTest, DeclarationTest){
    
}

int main(int argc, char *argv[]){
    
	UnitTest::TestReporterStdout reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}
