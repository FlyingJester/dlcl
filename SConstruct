dlcl = StaticLibrary("dlcl", ["dlcl_parser.cpp", "dlcl_lexer.cpp", "dlcl_std.cpp", "dlcl_utils.cpp"],
    CCFLAGS = " -g -O2 -Wall -Wextra ")
dlcl_test = Program(["test1.cpp"], LIBS = [dlcl], CCFLAGS = " -g -O2 ")
