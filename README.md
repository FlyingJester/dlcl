DLCL
====

DLCL is an imperative, procedural language with functional features. This implementation is
purely interpreted and intended to run on desktop machines as well as embedded platforms. It
has no dependencies, does not use any IO libraries (except in the included example), and has
constant memory requirements. Performance is deterministic and predictable.

DLCL is intended for short scripts, up to around 100 lines. It has constant memory usage,
but will OOM on long running scripts that perform many string operations. Scripts that only
perform integer and logic operations can be arbitrarily long running, however.

DLCL is licensed under the MPL v2.

The tests require UnitTest++, which uses an ISC/BSD style license. For convenience, a copy
of UnitTest++ (with readme and license text) is included in the unittest-cpp directory)
