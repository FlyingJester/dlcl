// This file is a part of UnitTest++. It is included here because it has been removed from later versions.
#ifndef UNITTEST_REQUIREMACROS_H
#define UNITTEST_REQUIREMACROS_H

#ifdef REQUIRE
    #error UnitTest++ redefines REQUIRE
#endif

#define REQUIRE for(UnitTest::RequiredCheckTestReporter decoratedReporter(*UnitTest::CurrentTest::Results()); decoratedReporter.Next(); )

#endif
