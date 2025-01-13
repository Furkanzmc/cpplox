#include "defs.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

SCENARIO("Test utility classes", "[lox++::utils]")
{
    struct PtrType {
        int& counter;

        PtrType(int& ctr)
          : counter{ ctr }
        {
            std::clog << "PtrType{}\n";
            counter++;
        }

        ~PtrType()
        {
            std::clog << "~PtrType{}\n";
            counter--;
        }
    };

    int counter{};
    {
        lox::copyable<PtrType*> ptrCopy{ counter };
    }
    CHECK(counter == 0);
}
