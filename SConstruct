import os

dlcl = StaticLibrary("dlcl", ["dlcl_parser.cpp", "dlcl_lexer.cpp", "dlcl_std.cpp", "dlcl_utils.cpp"],
    CCFLAGS = " -g -O2 -Wall -Wextra ")
dlcl_test1 = Program(["test1.cpp"], LIBS = [dlcl], CCFLAGS = " -g -O2 ")

dlcl_tests = Program(["dlcl_test.cpp"], LIBS = [dlcl, "unittest++"],
    LIBPATH = [os.getcwd()], CCFLAGS = " -g -O2 ",
    CPPPATH = [os.path.join(os.getcwd(), "unittest-cpp/UnitTest++")])
