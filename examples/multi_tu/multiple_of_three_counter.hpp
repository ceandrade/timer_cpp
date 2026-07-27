/******************************************************************************
 * @file: multiple_of_three_counter.hpp
 * @brief: Interface for the multiple-of-three worker.
 *
 * Declares the second worker used by the multi-translation-unit usage
 * example. Like the even counter, it simulates a module of a larger program
 * that shares the ExecutionStopper deadline with other, independently
 * compiled modules.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2026-07-27 by ceandrade.
 * Last update: 2026-07-27 by ceandrade.
 *****************************************************************************/

#pragma once

#include <cstddef>

/**
 * \brief Count how many random numbers drawn are multiples of three.
 *
 * Seeds a Mersenne Twister generator with a fixed seed and, on each
 * iteration, draws one pseudo-random integer and tests whether it is
 * congruent to zero modulo three. The loop runs until the shared
 * ExecutionStopper deadline expires.
 *
 * \return the number of values divisible by three drawn before the deadline
 *         expired.
 */
[[nodiscard]] std::size_t countMultiplesOfThree();
