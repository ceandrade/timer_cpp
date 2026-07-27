/******************************************************************************
 * @file test_timer.cpp
 * @brief Testing code for the Timer class.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2015-06-17 by ceandrade.
 * Last update: 2026-07-26 by ceandrade.
 ******************************************************************************/

#include "timer/timer.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

//-------------------------------[ Assert ]-----------------------------------//

// In some compilers, the `assert` function in header <cassert>
// is emptied defined. So, we just redefined it here
// (literally, we copied the code from `assert.h`).
#undef assert
#undef __assert
#define assert(e) \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))
#define __assert(e, file, line) \
    ((void)printf ("%s:%d: failed assertion `%s'\n", file, line, e), abort())

//--------------------------------[ Main ]------------------------------------//

int main() {
    cea::Timer timer;

    cout
    << "- After instantiation, the timer must be stopped: "
    << (timer.isStopped()? "OK" : "FAILED")
    << endl;
    assert(timer.isStopped());

    cout
    << "- And the elapsed time must be zero: " << timer.elapsed()
    << endl;
    assert(timer.elapsed() == 0s);

    timer.start();

    cout
    << "- After start, the timer must be running: "
    << (!timer.isStopped()? "OK" : "FAILED")
    << endl;
    assert(!timer.isStopped());

    cout << "- Sleep for 2 seconds..." << endl;
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 2.1s);

    cout << "- Sleep for more 2 seconds..." << endl;
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 4.1s);

    timer.stop();
    auto elapsed = timer.elapsed();

    cout << "- Stopping: " << (timer.isStopped()? "OK" : "FAILED") << endl;
    assert(timer.isStopped());

    cout << "- Elapsed time: " << timer.elapsed() << endl;

    cout << "- Sleep for 5 seconds..." << endl;
    std::this_thread::sleep_for(5s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() - elapsed <= 0s);

    cout << "- Resume the timer, and sleep for more 2 seconds..." << endl;
    timer.resume();
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 6.1s);

    cout << "All tests passed";
    return 0;
}
