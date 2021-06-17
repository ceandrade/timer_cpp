/******************************************************************************
 * test_timer.cpp: Testing code for timer class.
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

#include "timer/timer.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

//-------------------------------[ Assert ]-----------------------------------//

// In some compiler, the assert on <cassert> is emptied define. So, we just
// redefined it here (leterally, copied the code from assert.h)
#undef assert
#undef __asser
#define assert(e)  \
    ((void) ((e) ? ((void)0) : __assert (#e, __FILE__, __LINE__)))
#define __assert(e, file, line) \
    ((void)printf ("%s:%d: failed assertion `%s'\n", file, line, e), abort())

//--------------------------------[ Main ]------------------------------------//

int main(int argc, char* argv[]) {
    // using my_second = std::chrono::duration<double>;

    // const auto before = std::chrono::steady_clock::now();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    // const auto after = std::chrono::steady_clock::now();

    // my_second delta = after - before;

    // cout << "\n\n" << delta.count() << endl;


    cea::Timer timer;

    cout
    << "- After instantiation, the timer must be stopped: "
    << (timer.is_stopped()? "OK" : "FAILED")
    << endl;
    assert(timer.is_stopped());

    timer.start();

    cout
    << "- After start, the timer must be running: "
    << (!timer.is_stopped()? "OK" : "FAILED")
    << endl;
    assert(!timer.is_stopped());

    cout << "- Sleep for 2 seconds..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 2.1);

    cout << "- Sleep for more 2 seconds..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 4.1);

    timer.stop();
    auto elapsed = timer.elapsed();

    cout << "- Stopping: " << (timer.is_stopped()? "OK" : "FAILED") << endl;
    assert(timer.is_stopped());

    cout << "- Elapsed time: " << timer.elapsed() << endl;

    cout << "- Sleep for 5 seconds..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() - elapsed < 1e-6);

    cout << "- Resume the timer, and sleep for more 2 seconds..." << endl;
    timer.resume();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << "- Elapsed time: " << timer.elapsed() << endl;
    assert(timer.elapsed() < 6.1);

    cout << "All tests passed";
    return 0;
}
