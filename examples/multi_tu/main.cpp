/******************************************************************************
 * @file: main.cpp
 * @brief: Driver for the multi-translation-unit usage example.
 *
 * Sets a single shared deadline via the header-only ExecutionStopper
 * singleton, then runs two worker functions (each defined in its own,
 * separately-compiled translation unit) concurrently. Both workers observe
 * the same deadline, stop together, and return their tallies, which the
 * driver then reports.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2026-07-27 by ceandrade.
 * Last update: 2026-07-27 by ceandrade.
 *****************************************************************************/

#include "timer/execution_stopper.hpp"

#include "even_counter.hpp"
#include "multiple_of_three_counter.hpp"

#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    using cea::ExecutionStopper;

    std::cout << "Setting a shared 3-second deadline." << std::endl;
    ExecutionStopper::setExpirationTime(3s);
    ExecutionStopper::start();

    // Launch both workers concurrently; each runs in its own translation
    // unit but shares the same ExecutionStopper deadline.
    auto even_future =
        std::async(std::launch::async, countEvenNumbers);
    auto multiples_future =
        std::async(std::launch::async, countMultiplesOfThree);

    const std::size_t evens = even_future.get();
    const std::size_t multiples = multiples_future.get();

    std::cout
    << "\nResults:"
    << "\n  Even numbers drawn:       " << evens
    << "\n  Multiples of three drawn: " << multiples
    << "\n  Final elapsed:            "
    << ExecutionStopper::elapsed().count() << "s"
    << std::endl;

    return 0;
}
