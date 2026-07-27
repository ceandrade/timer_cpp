/******************************************************************************
 * @file: even_counter.cpp
 * @brief: Implementation of the even-number counting worker.
 *
 * One translation unit of the multi-TU example. It only queries the shared
 * header-only ExecutionStopper singleton; it never creates an instance of its
 * own, yet observes the very same deadline as the other workers.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2026-07-27 by ceandrade.
 * Last update: 2026-07-27 by ceandrade.
 *****************************************************************************/

#include "even_counter.hpp"

#include "timer/execution_stopper.hpp"

#include <iostream>
#include <random>
#include <thread>

using namespace std::chrono_literals;

std::size_t countEvenNumbers() {
    // Fixed seed for reproducibility.
    std::mt19937 rng {7512493};
    std::uniform_int_distribution<int> dist {0, 99};

    std::size_t evens = 0;

    // Loop until the shared deadline expires. isExpired() is a cheap atomic
    // load on the single, program-wide singleton instance.
    while(!cea::ExecutionStopper::isExpired()) {
        const int value = dist(rng);
        if(value % 2 == 0)
            ++evens;

        std::cout
        << "[even] drew " << value
        << ", elapsed = " << cea::ExecutionStopper::elapsed().count() << "s"
        << std::endl;

        std::this_thread::sleep_for(500ms);
    }

    return evens;
}
