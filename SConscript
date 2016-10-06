Import("environment")

import os

dlcl = environment.StaticLibrary("dlcl", ["dlcl_parser.cpp", "dlcl_lexer.cpp", "dlcl_std.cpp", "dlcl_utils.c"])
dlcl_test1 = environment.Program(["test1.cpp"], LIBS = [dlcl])

conf = Configure(environment)

if conf.CheckLib("UnitTest++"):
    has_test_lib = True
elif os.name == "posix":
    has_test_lib = os.path.isfile("libUnitTest++.a") or os.path.isfile("libUnitTest++.so")
else:
    has_test_lib = os.path.isfile("UnitTest++.lib")

conf.Finish()

if has_test_lib:
    test_env = environment.Clone()
    if os.path.exists("unittest-cpp/UnitTest++"):
        test_env.Append(CPPPATH = [os.path.join(os.getcwd(), "unittest-cpp/UnitTest++")], CPPDEFINES = ["DLCL_UNITTEST_LOCAL"])
    dlcl_tests = test_env.Program(["dlcl_test.cpp"], LIBS = [dlcl, "unittest++"],
        LIBPATH = [os.getcwd()], CCFLAGS = " -g -Os ")
else:
    print("WARNING: Skipping tests, you don't have UnitTest++ installed or in the build directory")

Return("dlcl")
