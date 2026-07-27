/******************************************************************************
 * @file: multiple_of_three_counter.cpp
 * @brief: Implementation of the multiple-of-three counting worker.
 *
 * A second translation unit of the multi-TU example, independently compiled
 * from even_counter.cpp and main.cpp. It shares the same header-only
 * ExecutionStopper singleton, proving there is a single deadline across all
 * translation units.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2026-07-27 by ceandrade.
 * Last update: 2026-07-27 by ceandrade.
 *****************************************************************************/

#include "multiple_of_three_counter.hpp"

#include "timer/execution_stopper.hpp"

#include <iostream>
#include <random>
#include <thread>

using namespace std::chrono_literals;

std::size_t countMultiplesOfThree() {
    // Fixed seed for reproducibility.
    std::mt19937 rng {7512493};
    std::uniform_int_distribution<int> dist {0, 99};

    std::size_t multiples = 0;

    // Independent worker sharing the same deadline as the even counter.
    while(!cea::ExecutionStopper::isExpired()) {
        const int value = dist(rng);
        if(value % 3 == 0)
            ++multiples;

        std::cout
        << "[mod3] drew " << value
        << ", elapsed = " << cea::ExecutionStopper::elapsed().count() << "s"
        << std::endl;

        std::this_thread::sleep_for(700ms);
    }

    return multiples;
}
