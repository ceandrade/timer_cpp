/******************************************************************************
 * timer.hpp: Interface for Timer class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2021
 *     All Rights Reserved.
 *
 *  Created on : Jun 17, 2021 by ceandrade
 *  Last update: Jun 17, 2021 by ceandrade
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
 * This class is simple timer class partially cloned from Boost::timer. The
 * objective is to have a steady wallclock-only timer without Boost dependency.
 */
class Timer {
public:
    /** \name Constructor and destructor */
    //@{
    /** Default Constructor.
     *
     * Load instance from a unique file.
     * \param filename from where the data will be loaded.
     */
    Timer(): start_time(), time_duration(0), stopped(true) {}
    //@}

public:
    /** Timer manipulation */
    //{@
    /// Start the timer. It also works as a reset.
    void start() noexcept {
        stopped = false;
        time_duration = nanoseconds(0);
        start_time = steady_clock::now();
    }

    /// Stop the timer.
    void stop() noexcept {
        if(stopped)
            return;
        stopped = true;
        time_duration += steady_clock::now() - start_time;
    }

    /// Resume the timer.
    void resume() noexcept {
        if(!stopped)
            return;
        start_time = steady_clock::now();
        stopped = false;
    }
    //@}

    /** Time retrieval */
    //@{
    /// Return the elapsed time between starts and stops in seconds.
    double elapsed() const noexcept {
        if(stopped)
            return duration<double>(time_duration).count();
        const auto delta = (steady_clock::now() - start_time) + time_duration;
        return duration<double>(delta).count();
    }

    /// Return true if the timer has been stopped.
    bool is_stopped() const noexcept {
        return stopped;
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
    bool stopped;
    //@}
};
} // end of namespace cea

#endif //CEA_TIMER_HPP_
