/******************************************************************************
 * execution_stopper.hpp: Interface for ExecutionStopper class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2021, 2025. All Rights Reserved..
 *
 *  Created on : May 19, 2015 by ceandrade
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

#ifndef CEA_EXECUTIONSTOPPER_HPP_
#define CEA_EXECUTIONSTOPPER_HPP_

#include "timer/timer.hpp"

#include <chrono>

namespace cea {

/**
 * \brief ExecutionStopper class.
 *
 * \author Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * \date 2025
 *
 * This class is a singleton designed to halt algorithm execution due to
 * either elapsed time or user intervention. It features a timer that can be
 * controlled through external calls. Additionally, it overrides
 * the Ctrl-C signal handling to enable a smoother termination of the algorithm.
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

        /** Set the expiration time to stop.
         * \param expiration_time the expiration time in seconds.
         */
        static void setExpirationTime(std::chrono::seconds expiration_time) noexcept;
        //@}

        /** Time retrieval */
        //@{
        /// Returns the elapsed time in seconds.
        static std::chrono::seconds elapsed() noexcept;

        /// Returns the elapsed time in seconds.
        static std::chrono::nanoseconds elapsedInNanoseconds() noexcept;

        /// Return true if the timer has been stopped.
        static bool isStopped() noexcept;

        // Indicate whether the timer is expired or we must stop due to SIGINT.
        static bool isExpired() noexcept;
        //@}

        /// Reset the time and stop status.
        // static void reset();
        //@}

    private:
        /** \name Private methods to avoid creation and copy */
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

        /// Function used to emit the STOP signal (Ctr-C).
        static void userSignalBreak(int signum);
        //@}

        /** \name Data members */
        //@{
        /// The maximum or expiration time in seconds.
        std::chrono::seconds expiration_time;

        /// The timer.
        cea::Timer timer;

        /// Holds a pointer to the previous Ctrl-C handler.
        void (*previousHandler)(int);

        /// Indicates if a STOP signal was emitted.
        bool stopsign;
        //@}
};
} // end of namespace cea

#endif //CEA_EXECUTIONSTOPPER_HPP_
