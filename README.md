:clock4: Simple timer and execution stopper 2.0
================================================================================

[![CI](https://github.com/ceandrade/timer_cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/ceandrade/timer_cpp/actions/workflows/ci.yml)

This project implements a simple timer and a companion execution stopper using
a steady wall clock and only standard C++ libraries (no Boost, no platform
dependent code). It was inspired by `Boost::timer`, but keeps a minimal,
header-friendly footprint.

Two classes are provided:

- `Timer` is a single instantiable stopwatch, implemented as header-only code.
  It can be started, stopped, resumed, and queried for elapsed time.

- `ExecutionStopper` is a singleton intended to be used as a single
  timekeeper shared across several modules. It halts algorithm execution due
  to either an elapsed time budget or user intervention (Ctrl-C). This class
  was designed with multiple threads in mind, where one needs to stop all
  the threads graciously.

:rocket: Usage
--------------------------------------------------------------------------------

The library is **header-only**. There is nothing to build or link: just put the
repository root on your include path and include the headers you need:

```cpp
#include "timer/timer.hpp"             // the Timer stopwatch
#include "timer/execution_stopper.hpp" // the ExecutionStopper singleton
```

Because `ExecutionStopper` spawns a background watchdog thread, you must also
link the system threads library. With GCC or Clang this simply means adding
`-pthread`:

```sh
g++ -std=c++23 -pthread -I/path/to/timer_cpp your_code.cpp -o your_app
```

On MSVC, threading is handled automatically; only the include path is needed:

```bat
cl /std:c++latest /EHsc /I C:\path\to\timer_cpp your_code.cpp
```

> **Note.** `ExecutionStopper` reports itself as *expired* when **either** the
> deadline you set with `setExpirationTime()` elapses **or** the application
> receives a `Ctrl-C` (`SIGINT`) signal. In both cases `isExpired()` starts
> returning `true`, so a single check covers both a time budget and a user
> interruption, letting every worker wind down gracefully.

Two complete, ready-to-build examples are provided, each in its own folder with
a small `Makefile`.

### Example 1: single translation unit

[`examples/single_tu`](examples/single_tu) is the simplest possible use: it
sets a 10-second deadline, then loops printing the elapsed time once per second
until the stopper expires (either the 10 seconds elapse, or you press
`Ctrl-C`).

```cpp
#include "timer/execution_stopper.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {
    using cea::ExecutionStopper;

    // Set a 10-second deadline and start the shared timer.
    ExecutionStopper::setExpirationTime(10s);
    ExecutionStopper::start();

    // Loop until the deadline expires (or Ctrl-C is pressed), printing the
    // elapsed time each second.
    while(!ExecutionStopper::isExpired()) {
        std::cout
        << "Elapsed: " << ExecutionStopper::elapsed().count() << "s"
        << std::endl;
        std::this_thread::sleep_for(1s);
    }

    std::cout
    << "Deadline reached after "
    << ExecutionStopper::elapsed().count() << "s. Done."
    << std::endl;

    return 0;
}
```

Build and run it:

```sh
cd examples/single_tu
make run                 # or: make CXX=clang++ run
```

### Example 2: multiple translation units

[`examples/multi_tu`](examples/multi_tu) simulates a larger program split
across several independently-compiled translation units. Because
`ExecutionStopper` is a header-only singleton, every translation unit shares
**the same** deadline and *expired* state — there is exactly one instance for
the whole program.

The driver sets a single shared deadline and launches two workers
concurrently:

- [`even_counter.cpp`](examples/multi_tu/even_counter.cpp) —
  `countEvenNumbers()` draws pseudo-random numbers and counts how many are
  even, until the shared deadline expires.
- [`multiple_of_three_counter.cpp`](examples/multi_tu/multiple_of_three_counter.cpp) —
  `countMultiplesOfThree()` does the same, counting multiples of three.

Each worker is declared in its own header
([`even_counter.hpp`](examples/multi_tu/even_counter.hpp),
[`multiple_of_three_counter.hpp`](examples/multi_tu/multiple_of_three_counter.hpp))
and returns its tally, which the driver reports:

```cpp
#include "timer/execution_stopper.hpp"

#include "even_counter.hpp"
#include "multiple_of_three_counter.hpp"

#include <cstddef>
#include <future>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    using cea::ExecutionStopper;

    // A single deadline, shared by every translation unit. It fires when the
    // 3 seconds elapse or when the application receives a Ctrl-C signal.
    ExecutionStopper::setExpirationTime(3s);
    ExecutionStopper::start();

    // Both workers observe the same deadline and stop together.
    auto even_future =
        std::async(std::launch::async, countEvenNumbers);
    auto multiples_future =
        std::async(std::launch::async, countMultiplesOfThree);

    const std::size_t evens = even_future.get();
    const std::size_t multiples = multiples_future.get();

    std::cout
    << "\nResults:"
    << "\n  Even numbers drawn:       " << evens
    << "\n  Multiples of three drawn: " << multiples
    << std::endl;

    return 0;
}
```

Each source file is compiled separately and then linked together — see the
[`Makefile`](examples/multi_tu/Makefile). Build and run it:

```sh
cd examples/multi_tu
make run                 # or: make CXX=clang++ run
```

If you press `Ctrl-C` while either example is running, the stopper expires
immediately and the program winds down cleanly instead of being killed
mid-work.

:gear: How it works
--------------------------------------------------------------------------------

This section describes `ExecutionStopper`. It is a singleton, so there is a
single deadline and a single "expired" state shared by the whole program.

### The watchdog

When you start the stopper (and set an expiration time), it spawns a
background *watchdog* thread that sleeps until the deadline is reached. When
the deadline elapses, the watchdog sets an atomic `expired` flag. Querying the
stopper via `isExpired()` is therefore a single relaxed atomic load — no clock
is read, and no system call is performed.

The older version worked the other way around: every call to `isExpired()`
read the system clock and compared it against the deadline. That is fine for a
single thread, but it becomes expensive when `isExpired()` is called very
frequently, and especially when it is called from many threads at once: each
call is a clock query, and the clock is a shared resource. The current design
turns those frequent checks into cheap atomic loads, so the cost is paid once,
in the background, by the watchdog — not on every check.

This is the main advantage of the singleton design: **one stopwatch for all
threads, stopping them all at the same time, without a flood of system calls.**
No matter how many threads run tight loops calling `isExpired()`, they only
read a shared atomic flag.

`ExecutionStopper` also overrides the `SIGINT` (Ctrl-C) handler so that a user
interrupt sets the same `expired` flag. This lets you abort a long, multi-
threaded computation cleanly: every thread sees `isExpired() == true` and can
wind down gracefully, rather than the process being killed mid-work.

### A typical multithreaded use

A common scenario is a parallel heuristic (e.g., a metaheuristic running
several independent searches) where each thread runs nested outer/inner loops
and checks the shared deadline in the innermost loop:

```cpp
#include "timer/execution_stopper.hpp"
#include <thread>
#include <vector>

using cea::ExecutionStopper;

void search() {
    // Outer loop: restarts / perturbations.
    while(!ExecutionStopper::isExpired()) {
        // Inner loop: local search moves.
        for(std::size_t i = 0; i < num_moves; ++i) {
            if(ExecutionStopper::isExpired())  // cheap atomic load.
                return;
            // ... evaluate move, update incumbent ...
        }
    }
}

ExecutionStopper::setExpirationTime(std::chrono::seconds{60});
ExecutionStopper::start();

std::vector<std::thread> pool;
for(unsigned t = 0; t < num_threads; ++t)
    pool.emplace_back(search);

for(auto& worker : pool)
    worker.join();
```

All worker threads observe the same 60-second budget and stop together, while
`isExpired()` stays cheap even though it is hammered from every thread.

### Advantages and drawbacks

Advantages:

- Frequent `isExpired()` checks are just atomic loads, so they scale well
  across many threads without system-call contention.
- A single shared deadline stops every thread at (approximately) the same
  time.
- Ctrl-C is folded into the same mechanism, enabling clean, cooperative
  shutdown of multithreaded runs.

Drawbacks:

- It spawns a background watchdog thread (a small, one-per-lifecycle resource
  cost).
- Expiration is only as precise as the OS scheduler can wake the watchdog, so
  it is **not** suited to nanosecond-level deadlines. For coarse budgets —
  seconds or minutes, which is the case for most optimization algorithms and
  heuristics — this is perfectly adequate.
- The `expired` flag is read and written with relaxed atomics, so there is a
  tiny cross-core visibility delay. Again, at the granularity of seconds or
  minutes this is irrelevant.
- The `SIGINT` override is process-wide. If another library "steals" `SIGINT`
  (for instance, some solver libraries install their own handler), and it does
  so *after* `ExecutionStopper` is constructed, then Ctrl-C will no longer
  behave as intended.

:black_nib: License and Citing
--------------------------------------------------------------------------------

This project uses a permissive BSD-like license and it can be used as it
pleases you.

[Check it out the full license.](https://github.com/ceandrade/timer_cpp/blob/master/LICENSE.md)

:pencil2: Contributing
--------------------------------------------------------------------------------

[Contribution guidelines for this project](CONTRIBUTING.md)
