/******************************************************************************
 * @file: main.cpp
 * @brief: Single translation unit usage example for the timer_cpp library.
 *
 * Minimal demonstration of the header-only ExecutionStopper: set a deadline,
 * start the timer, and loop reporting the elapsed time once per second until
 * the deadline expires.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2026-07-27 by ceandrade.
 * Last update: 2026-07-27 by ceandrade.
 *****************************************************************************/

#include "timer/execution_stopper.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {
    using cea::ExecutionStopper;

    // Set a 10-second deadline and start the shared timer.
    ExecutionStopper::setExpirationTime(10s);
    ExecutionStopper::start();

    // Loop until the deadline expires, printing the elapsed time each second.
    while(!ExecutionStopper::isExpired()) {
        std::cout
        << "Elapsed: " << ExecutionStopper::elapsed().count() << "s"
        << std::endl;
        std::this_thread::sleep_for(1s);
    }

    std::cout
    << "Deadline reached after "
    << ExecutionStopper::elapsed().count() << "s. Done."
    << std::endl;

    return 0;
}
