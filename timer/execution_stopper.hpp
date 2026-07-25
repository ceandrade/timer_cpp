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
#include <csignal>
#include <iostream>
#include <limits>
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

//----------------------------------------------------------------------------//
// Inline implementation (header-only).
//
// All definitions below are `inline`, so including this header in several
// translation units is ODR-safe: the linker folds every copy into a single
// definition. The singleton itself is a Meyers singleton (function-local
// static inside the `inline` instance() below), which C++ guarantees to be a
// single, thread-safely initialized object shared by every translation unit.
//----------------------------------------------------------------------------//

//------------------[ Default Constructor and Destructor ]--------------------//

inline ExecutionStopper::ExecutionStopper():
    // NOTE: we cannot setup the maximum expiration time as
    // seconds::max() because some libraries return 0 instead the max,
    // or use nanoseconds::max() because it would overflow.
    expiration_time {3600 * 24 * 365},  // 1 year
    timer {},
    previousHandler {std::signal(SIGINT, userSignalBreak)},
    expired {false},
    watchdog_mutex {},
    watchdog_cv {},
    watchdog_cancelled {false},
    watchdog {}
{}

inline ExecutionStopper::~ExecutionStopper() {
    watchdog_cancelled.store(true, std::memory_order_relaxed);
    watchdog_cv.notify_all();
    if(watchdog.joinable())
        watchdog.join();
}

//--------------------[ Singleton instance initialization ]-------------------//

inline ExecutionStopper& ExecutionStopper::instance() {
    static ExecutionStopper inst;
    return inst;
}

//---------------------[ Watchdog thread management ]-------------------------//

inline void ExecutionStopper::spawnWatchdog() noexcept {
    // Destroy any existing watchdog (signal cancel + join).
    watchdog_cancelled.store(true, std::memory_order_relaxed);
    watchdog_cv.notify_all();
    if(watchdog.joinable())
        watchdog.join();
    watchdog_cancelled.store(false, std::memory_order_relaxed);

    // Maximum seconds safely representable as nanoseconds.
    static constexpr auto max_safe_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::nanoseconds::max());

    // Don't spawn if no meaningful deadline is set.
    if(expiration_time >= max_safe_seconds)
        return;

    // Compute remaining time until deadline.
    const auto total_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(expiration_time);
    const auto elapsed_ns = timer.elapsedInNanoseconds();
    const auto remaining = total_ns - elapsed_ns;

    // Already expired.
    if(remaining <= std::chrono::nanoseconds{0}) {
        expired.store(true, std::memory_order_relaxed);
        return;
    }

    // Spawn a watchdog that sleeps until deadline or explicit cancellation.
    watchdog = std::thread{[this, remaining] {
        std::unique_lock lock(watchdog_mutex);
        const bool cancelled = watchdog_cv.wait_for(
            lock, remaining,
            [this] {
                return watchdog_cancelled.load(std::memory_order_relaxed);
            }
        );
        if(!cancelled)
            expired.store(true, std::memory_order_relaxed);
    }};
}

//--------------------------[ Timer manipulation ]----------------------------//

inline void ExecutionStopper::start() noexcept {
    auto& inst = instance();
    inst.expired.store(false, std::memory_order_relaxed);
    inst.timer.start();
    inst.spawnWatchdog();
}

inline void ExecutionStopper::stop() noexcept {
    auto& inst = instance();
    inst.timer.stop();
    // Kill watchdog (signal cancel + join).
    inst.watchdog_cancelled.store(true, std::memory_order_relaxed);
    inst.watchdog_cv.notify_all();
    if(inst.watchdog.joinable())
        inst.watchdog.join();
}

inline void ExecutionStopper::resume() noexcept {
    auto& inst = instance();
    // If already expired, nothing to resume.
    if(inst.expired.load(std::memory_order_relaxed))
        return;
    inst.timer.resume();
    inst.spawnWatchdog();
}

inline void ExecutionStopper::setExpirationTime(
    std::chrono::seconds expiration_time) noexcept
{
    auto& inst = instance();
    inst.expiration_time = expiration_time;
    // If the timer is running, restart the watchdog with the new deadline.
    if(!inst.timer.isStopped())
        inst.spawnWatchdog();
}

//----------------------------[ Time retrieval ]------------------------------//

inline std::chrono::seconds ExecutionStopper::elapsed() noexcept {
    return instance().timer.elapsed();
}

inline std::chrono::nanoseconds
ExecutionStopper::elapsedInNanoseconds() noexcept {
    return instance().timer.elapsedInNanoseconds();
}

inline bool ExecutionStopper::isStopped() noexcept {
    return instance().timer.isStopped();
}

//-----------------------------[ Timer expired ]------------------------------//

inline bool ExecutionStopper::isExpired() noexcept {
    return instance().expired.load(std::memory_order_relaxed);
}

//----------------------------[ Ctrl-C handler ]------------------------------//

inline void ExecutionStopper::userSignalBreak(int /*signum*/) {
    instance().expired.store(true, std::memory_order_relaxed);
    std::signal(SIGINT, instance().previousHandler);
    std::cerr << "\n\n> Ctrl-C detected. Aborting execution. "
              << "Type Ctrl-C once more for exit immediately."
              << std::endl;
}

} // end of namespace cea
