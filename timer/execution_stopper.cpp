/******************************************************************************
 * execution_stopper.cpp: Implementation for ExecutionStopper class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2021, 2025. All Rights Reserved.
 *
 *  Created on : May 19, 2015 by andrade
 *  Last update: Apr 08, 2025 by andrade
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

#include <chrono>

namespace cea {

//------------------[ Default Constructor and Destructor ]--------------------//

ExecutionStopper::ExecutionStopper():
    expiration_time {std::numeric_limits<decltype(expiration_time)>::max()},
    timer {},
    previousHandler {signal(SIGINT, userSignalBreak)},
    stopsign {false}
{}

ExecutionStopper::~ExecutionStopper() {}

//--------------------[ Singleton instance initialization ]-------------------//

ExecutionStopper& ExecutionStopper::instance() {
    static ExecutionStopper inst;
    return inst;
}

//--------------------------[ Timer manipulation ]----------------------------//

void ExecutionStopper::start() noexcept {
    instance().timer.start();
}

void ExecutionStopper::stop() noexcept {
    instance().timer.stop();
}

void ExecutionStopper::resume() noexcept {
    instance().timer.resume();
}

void ExecutionStopper::setExpirationTime(std::chrono::seconds expiration_time)
    noexcept
{
    instance().expiration_time = expiration_time;
}

//----------------------------[ Time retrieval ]------------------------------//

std::chrono::seconds ExecutionStopper::elapsed() noexcept {
    return instance().timer.elapsed();
}

std::chrono::nanoseconds ExecutionStopper::elapsedInNanoseconds() noexcept {
    return instance().timer.elapsedInNanoseconds();
}

bool ExecutionStopper::isStopped() noexcept {
    return instance().timer.isStopped();
}

//-----------------------------[ Timer expired ]------------------------------//

bool ExecutionStopper::isExpired() noexcept {
    const auto &inst = instance();
    return (inst.timer.elapsed() > inst.expiration_time) || inst.stopsign;
}

//----------------------------[ Ctrl-C handler ]------------------------------//

void ExecutionStopper::userSignalBreak(int /*signum*/) {
    instance().stopsign = true;
    signal(SIGINT, instance().previousHandler);
    std::cerr << "\n\n> Ctrl-C detected. Aborting execution. "
              << "Type Ctrl-C once more for exit immediately."
              << std::endl;
}

} // end of namespace cea
