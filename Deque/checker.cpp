#include "common.hpp"
#include "deque.hpp"

#include <ctime>
#include <iostream>
#include <deque>
#include <random>

std::default_random_engine randnum(time(NULL));

static const int N = 20000;

int main() {
    bool (*testFunc[])()= {
        pushTest, popTest, insertTest, iteratorTest,
        eraseTest, copyAndClearTest, memoryTest,
        nomercyTest,
    };

    const char *testMessage[] = {
        "Testing push...", "Testing pop...", "Testing insert...", "Testing iterator...",
        "Testing erase...", "Testing copy and clear...", "Testing memory...",
        "Final test without mercy...",
    };

    bool error = false;
    for (int i = 0; i < sizeof(testFunc) / sizeof(testFunc[0]); i++) {
        printf("%-40s", testMessage[i]);
        if (testFunc[i]())
            printf("Passed\n");
        else {
            error = true;
            printf("Failed !!!\n");
        }
    }

    if (error)
        printf("\nUnfortunately, you failed in this test\n\a");
    else
        printf("\nCongratulations, your deque passed all the tests!\n");

    return 0;

}
