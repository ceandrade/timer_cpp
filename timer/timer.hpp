/******************************************************************************
 * @file timer.hpp
 * @brief Interface for the Timer class.
 *
 * SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause.
 *
 * Created on : 2015-06-17 by ceandrade.
 * Last update: 2026-07-26 by ceandrade.
 ******************************************************************************/

#pragma once

#include <chrono>

namespace cea {

using namespace std::chrono;

/**
 * \brief Timer class.
 *
 * \author Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * \date 2026
 *
 * This class is a simple timer partially cloned from Boost::timer.
 * The objective is to have a steady wallclock-only timer without
 * a Boost dependency.
 */
class Timer {
public:
    /** \name Constructor and destructor */
    //@{
    /// Default Constructor.
    Timer(): start_time {}, time_duration {0}, is_stopped {true} {}
    //@}

public:
    /** Timer manipulation */
    //{@
    /// Start the timer. It also works as a reset.
    void start() noexcept {
        is_stopped = false;
        time_duration = nanoseconds {0};
        start_time = steady_clock::now();
    }

    /// Stop the timer.
    void stop() noexcept {
        if(is_stopped)
            return;
        is_stopped = true;
        time_duration += steady_clock::now() - start_time;
    }

    /// Resume the timer.
    void resume() noexcept {
        if(!is_stopped)
            return;
        start_time = steady_clock::now();
        is_stopped = false;
    }
    //@}

    /** Time retrieval */
    //@{
    /// Return the elapsed time between starts and stops in nanoseconds.
    std::chrono::nanoseconds elapsedInNanoseconds() const noexcept {
        if(is_stopped)
            return time_duration;
        const auto delta = (steady_clock::now() - start_time) + time_duration;
        return delta;
    }

    /// Return the elapsed time between starts and stops in seconds.
    std::chrono::seconds elapsed() const noexcept {
        return duration_cast<seconds>(this->elapsedInNanoseconds());
    }

    /// Return true if the timer has been stopped.
    bool isStopped() const noexcept {
        return is_stopped;
    }
    //@}

protected:
    /** \name Data members */
    //@{
    /// Holds thow much time has passed between timer starts and stops.
    time_point<steady_clock> start_time;

    /// Holds how much time has passed between timer starts and stops.
    nanoseconds time_duration;

    /// Indicates whether the timer is stopped or not.
    bool is_stopped;
    //@}
};
} // end of namespace cea
