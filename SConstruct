import os

environment = Environment(ENV = os.environ)
environment.Append(CCFLAGS = " -g ", LINKFLAGS = " -g ", CXXFLAGS = " -fno-rtti ")

SConscript(dirs = [os.getcwd()], exports = ["environment"])
