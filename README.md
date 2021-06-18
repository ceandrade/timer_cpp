:clock4: Simple timer and execution stopper
================================================================================

This project implements a simple timer class inspired on Boost::timer. However,
we use steady wallclock and only standard C++ libraries.

Class `Timer` is a single instantiable timer, implemented as header-only code.

Class `ExecutionStopper` is a singleton which intent is to be used across
several modules as a single time keeper. Therefore, you must compile and link
`execution_stopper.cpp` in your code, so that it becames a single
translation unit (module) visible to other translation units (modules) during
the linking phase.

Documentation is on the way. Meanwhile, please, refer to the code directly,
special the test cases that shows how to user these timers.

:black_nib: License and Citing
--------------------------------------------------------------------------------

This project uses a permissive BSD-like license and it can be used as it
pleases you.

[Check it out the full license.](https://github.com/ceandrade/timer/blob/master/LICENSE.md)

:pencil2: Contributing
--------------------------------------------------------------------------------

[Contribution guidelines for this project](CONTRIBUTING.md)
