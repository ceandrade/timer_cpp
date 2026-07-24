/******************************************************************************
 * execution_stopper.hpp: Interface for ExecutionStopper class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2015, 2026. All Rights Reserved.
 *
 * Created on : 2015-05-19 by ceandrade.
 * Last update: 2026-07-24 by ceandrade.
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

#pragma once

#include "timer/timer.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace cea {

/**
 * \brief ExecutionStopper class.
 *
 * \author Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * \date 2026
 *
 * This class is a singleton designed to halt algorithm execution due to
 * either elapsed time or user intervention. It features a timer that can be
 * controlled through external calls. Additionally, it overrides
 * the Ctrl-C signal handling to enable a smoother termination of the
 * algorithm.
 *
 * Expiration is implemented via a background watchdog thread
 * (std::thread) that sets an atomic flag when the deadline is reached.
 * The isExpired() check is therefore a single relaxed atomic load,
 * avoiding repeated clock syscalls in hot loops.
 *
 * NOTE: the default expiration time is set to one year (31,536,000 seconds).
 * This is a compromise between having a reasonably long default expiration time
 * and avoiding potential overflows. We cannot setup the maximum expiration time
 * as seconds::max() because some libraries return 0 instead of the max,
 * or use nanoseconds::max() because it would overflow.
 */
class ExecutionStopper {
    public:
        /** \name Public interface */
        //@{
        /** Timer manipulation */
        //@{
        /// Start the timer.
        static void start() noexcept;

        /// Stop the timer.
        static void stop() noexcept;

        /// Resume the timer.
        static void resume() noexcept;

        /**
         * \brief Set the expiration time to stop.
         * \param expiration_time the expiration time in seconds.
         *
         * If the timer is already running, the watchdog is restarted
         * with the updated deadline.
         */
        static void setExpirationTime(
            std::chrono::seconds expiration_time
        ) noexcept;
        //@}

        /** Time retrieval */
        //@{
        /// Returns the elapsed time in seconds.
        static std::chrono::seconds elapsed() noexcept;

        /// Returns the elapsed time in nanoseconds.
        static std::chrono::nanoseconds elapsedInNanoseconds() noexcept;

        /// Return true if the timer has been stopped.
        static bool isStopped() noexcept;

        /// Indicate whether the timer expired or we must stop due to SIGINT.
        static bool isExpired() noexcept;
        //@}

        /// Reset the time and stop status.
        // static void reset();
        //@}

    private:
        /** \name Private methods to avoid creation and copy */
        //@{
        ExecutionStopper();
        ExecutionStopper(const ExecutionStopper&) = delete;
        ~ExecutionStopper();
        ExecutionStopper& operator=(const ExecutionStopper&) = delete;
        //@}

    protected:
        /** \name Internals */
        //@{
        /// Get a reference for an instance.
        static ExecutionStopper& instance();

        /// Function used to emit the STOP signal (Ctrl-C).
        static void userSignalBreak(int signum);

        /// Spawn (or respawn) the watchdog thread for the remaining time.
        void spawnWatchdog() noexcept;
        //@}

        /** \name Data members */
        //@{
        /// The maximum or expiration time in seconds.
        std::chrono::seconds expiration_time;

        /// The timer.
        cea::Timer timer;

        /// Holds a pointer to the previous Ctrl-C handler.
        void (*previousHandler)(int);

        /// Indicates expiration (timeout or Ctrl-C).
        std::atomic<bool> expired;

        /// Mutex used by the watchdog condition variable.
        std::mutex watchdog_mutex;

        /// Condition variable for interruptible sleep in the watchdog.
        std::condition_variable_any watchdog_cv;

        /// Cancellation flag for watchdog thread wake up.
        std::atomic<bool> watchdog_cancelled;

        /// Background watchdog thread that sets the expired flag.
        std::thread watchdog;
        //@}
};
} // end of namespace cea
