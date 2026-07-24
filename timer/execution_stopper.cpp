/******************************************************************************
 * execution_stopper.cpp: Implementation for ExecutionStopper class.
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

#include "execution_stopper.hpp"

#include <csignal>
#include <iostream>
#include <limits>

namespace cea {

using namespace std::chrono;

//------------------[ Default Constructor and Destructor ]--------------------//

ExecutionStopper::ExecutionStopper():
    // NOTE: we cannot setup the maximum expiration time as
    // seconds::max() because some libraries return 0 instead the max,
    // or use nanoseconds::max() because it would overflow.
    expiration_time {3600 * 24 * 365},  // 1 year
    timer {},
    previousHandler {signal(SIGINT, userSignalBreak)},
    expired {false},
    watchdog_mutex {},
    watchdog_cv {},
    watchdog_cancelled {false},
    watchdog {}
{}

ExecutionStopper::~ExecutionStopper() {
    watchdog_cancelled.store(true, std::memory_order_relaxed);
    watchdog_cv.notify_all();
    if(watchdog.joinable())
        watchdog.join();
}

//--------------------[ Singleton instance initialization ]-------------------//

ExecutionStopper& ExecutionStopper::instance() {
    static ExecutionStopper inst;
    return inst;
}

//---------------------[ Watchdog thread management ]-------------------------//

void ExecutionStopper::spawnWatchdog() noexcept {
    // Destroy any existing watchdog (signal cancel + join).
    watchdog_cancelled.store(true, std::memory_order_relaxed);
    watchdog_cv.notify_all();
    if(watchdog.joinable())
        watchdog.join();
    watchdog_cancelled.store(false, std::memory_order_relaxed);

    // Maximum seconds safely representable as nanoseconds.
    static constexpr auto max_safe_seconds =
        duration_cast<seconds>(nanoseconds::max());

    // Don't spawn if no meaningful deadline is set.
    if(expiration_time >= max_safe_seconds)
        return;

    // Compute remaining time until deadline.
    const auto total_ns = duration_cast<nanoseconds>(expiration_time);
    const auto elapsed_ns = timer.elapsedInNanoseconds();
    const auto remaining = total_ns - elapsed_ns;

    // Already expired.
    if(remaining <= nanoseconds{0}) {
        expired.store(true, std::memory_order_relaxed);
        return;
    }

    // Spawn a watchdog that sleeps until deadline or explicit cancellation.
    watchdog = std::thread{[this, remaining] {
        std::unique_lock lock(watchdog_mutex);
        const bool cancelled = watchdog_cv.wait_for(
            lock, remaining,
            [this] { return watchdog_cancelled.load(std::memory_order_relaxed); }
        );
        if(!cancelled)
            expired.store(true, std::memory_order_relaxed);
    }};
}

//--------------------------[ Timer manipulation ]----------------------------//

void ExecutionStopper::start() noexcept {
    auto& inst = instance();
    inst.expired.store(false, std::memory_order_relaxed);
    inst.timer.start();
    inst.spawnWatchdog();
}

void ExecutionStopper::stop() noexcept {
    auto& inst = instance();
    inst.timer.stop();
    // Kill watchdog (signal cancel + join).
    inst.watchdog_cancelled.store(true, std::memory_order_relaxed);
    inst.watchdog_cv.notify_all();
    if(inst.watchdog.joinable())
        inst.watchdog.join();
}

void ExecutionStopper::resume() noexcept {
    auto& inst = instance();
    // If already expired, nothing to resume.
    if(inst.expired.load(std::memory_order_relaxed))
        return;
    inst.timer.resume();
    inst.spawnWatchdog();
}

void ExecutionStopper::setExpirationTime(
    seconds expiration_time) noexcept
{
    auto& inst = instance();
    inst.expiration_time = expiration_time;
    // If the timer is running, restart the watchdog with the new deadline.
    if(!inst.timer.isStopped())
        inst.spawnWatchdog();
}

//----------------------------[ Time retrieval ]------------------------------//

seconds ExecutionStopper::elapsed() noexcept {
    return instance().timer.elapsed();
}

nanoseconds ExecutionStopper::elapsedInNanoseconds() noexcept {
    return instance().timer.elapsedInNanoseconds();
}

bool ExecutionStopper::isStopped() noexcept {
    return instance().timer.isStopped();
}

//-----------------------------[ Timer expired ]------------------------------//

bool ExecutionStopper::isExpired() noexcept {
    return instance().expired.load(std::memory_order_relaxed);
}

//----------------------------[ Ctrl-C handler ]------------------------------//

void ExecutionStopper::userSignalBreak(int /*signum*/) {
    instance().expired.store(true, std::memory_order_relaxed);
    signal(SIGINT, instance().previousHandler);
    std::cerr << "\n\n> Ctrl-C detected. Aborting execution. "
              << "Type Ctrl-C once more for exit immediately."
              << std::endl;
}

} // end of namespace cea
