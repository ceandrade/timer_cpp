/******************************************************************************
 * timer.hpp: Interface for Timer class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2021, 2025. All Rights Reserved.
 *
 *  Created on : Jun 17, 2021 by ceandrade
 *  Last update: Apr 08, 2025 by ceandrade
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 ******************************************************************************/

#ifndef CEA_TIMER_HPP_
#define CEA_TIMER_HPP_

#include <chrono>

namespace cea {

using namespace std::chrono;

/**
 * \brief Timer class.
 *
 * \author Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * \date 2021
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

#endif //CEA_TIMER_HPP_
