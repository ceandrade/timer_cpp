/******************************************************************************
 * test_timer.cpp: Testing code for timer class.
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

#include "timer/timer.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

//-------------------------------[ Assert ]-----------------------------------//

// In some compilers, the `assert` function in header <cassert>
// is emptied defined. So, we just redefined it here
// (literally, we copied the code from `assert.h`).
#undef assert
#undef __assert
#define assert(e) \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))
#define __assert(e, file, line) \
    ((void)printf ("%s:%d: failed assertion `%s'\n", file, line, e), abort())

//--------------------------------[ Main ]------------------------------------//

int main() {
    cea::Timer timer;

    cout
    << "- After instantiation, the timer must be stopped: "
    << (timer.isStopped()? "OK" : "FAILED")
    << endl;
    assert(timer.isStopped());

    cout
    << "- And the elapsed time must be zero: " << timer.elapsed()
    << endl;
    assert(timer.elapsed() == 0s);

    timer.start();

    cout
    << "- After start, the timer must be running: "
    << (!timer.isStopped()? "OK" : "FAILED")
    << endl;
    assert(!timer.isStopped());

    cout << "- Sleep for 2 seconds..." << endl;
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 2.1s);

    cout << "- Sleep for more 2 seconds..." << endl;
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 4.1s);

    timer.stop();
    auto elapsed = timer.elapsed();

    cout << "- Stopping: " << (timer.isStopped()? "OK" : "FAILED") << endl;
    assert(timer.isStopped());

    cout << "- Elapsed time: " << timer.elapsed() << endl;

    cout << "- Sleep for 5 seconds..." << endl;
    std::this_thread::sleep_for(5s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() - elapsed <= 0s);

    cout << "- Resume the timer, and sleep for more 2 seconds..." << endl;
    timer.resume();
    std::this_thread::sleep_for(2s);
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 6.1s);

    cout << "All tests passed";
    return 0;
}
