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

Building tests requires UnitTest++. You can get this at: https://github.com/pjohnmeyer/unittest-cpp

Examples
--------

```
% Hello, world!
call Print: "Hello, world!\n".
```

```
% Hello world using alternative call syntax
[Print "Hello, world!\n"]
```

```
% Nintety-nine bottles of beer on the wall
int i 99
loop i > 0:
    [Print i " bottles of beer on the wall, " i " bottles of beer.\n"]
    [Print "Take one down, pass it around\n"]
    set i get i - 1
    [Print i " bottles of beer on the wall."]
.
```

```
% Imperative Fibonacci
int n 12 % Number to take
int i 0 % Iterator
int x 1 % Current Fibonacci number
int y 1 % Previous Fibonacci number
loop i < n:
    int z get x
    set x get y + get z
    set y get z
    set i get i + 1
.
```
