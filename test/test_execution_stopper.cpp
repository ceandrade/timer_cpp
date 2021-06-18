/******************************************************************************
 * test_timer.cpp: Testing code for timer class.
 *
 * Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
 * (c) Copyright 2021. All Rights Reserved.
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

#include "timer/execution_stopper.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

//-------------------------------[ Assert ]-----------------------------------//

// In some compiler, the assert on <cassert> is emptied define. So, we just
// redefined it here (leterally, copied the code from assert.h)
#undef assert
#undef __assert
#define assert(e)  \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))
#define __assert(e, file, line) \
    ((void)printf ("%s:%d: failed assertion `%s'\n", file, line, e), abort())

//--------------------------------[ Main ]------------------------------------//

int main(int argc, char* argv[]) {
    using exec = cea::ExecutionStopper;

    cout
    << "- After instantiation, the timer must not be expired: "
    << (!exec::isExpired()? "OK" : "FAILED")
    << endl;
    assert(!exec::isExpired());

    exec::setExpirationTime(5);
    exec::start();

    cout << "- Set expiration for 10 seconds. Sleep 2 seconds..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cout << "- Elapsed time: " << exec::elapsed() << endl;
    assert(exec::elapsed() < 2.1);

    cout << "- Stop and sleep 2 seconds..." << endl;
    exec::stop();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cout << "- Resume and sleep 2 seconds..." << endl;
    exec::resume();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cout << "- Elapsed time: " << exec::elapsed() << endl;
    assert(exec::elapsed() < 4.1);

    cout
    << "- Should not be expired yet: "
    << (!exec::isExpired()? "OK" : "FAILED")
    << endl;
    assert(!exec::isExpired());

    cout << "- Sleep 2 seconds for expiration..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cout
    << "- Should be expired by now: "
    << (exec::isExpired()? "OK" : "FAILED")
    << endl;
    assert(exec::isExpired());

    cout << "- Elapsed time: " << exec::elapsed() << endl;
    assert(exec::elapsed() < 6.1);

    exec::start();
    cout
    << "- After resetting, should not be expired..."
    << (!exec::isExpired()? "OK" : "FAILED")
    << endl;
    assert(!exec::isExpired());

    cout << "- Elapsed time: " << exec::elapsed() << endl;
    assert(exec::elapsed() > 0.0);

    cout << "All tests passed";
    return 0;
}
