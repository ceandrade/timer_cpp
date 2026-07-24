:clock4: Simple timer and execution stopper
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
  to either an elapsed time budget or user intervention (Ctrl-C). Because it
  is a singleton, you must compile and link `execution_stopper.cpp` in your
  project so that it becomes a single translation unit visible to every other
  translation unit during the linking phase.

:rocket: Usage
--------------------------------------------------------------------------------

*Coming soon.*

<!-- TODO: usage section (build, link, and API walkthrough). -->

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
