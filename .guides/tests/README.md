# CS104 HW5 Test Suite

The test suite supports CMake 3.10 and newer, including Codio's 3.10.2
and the student image's 3.16.3. The Python test runner supports Python 3.6.9
and newer using only the Python standard library.

### Using the Tests

Keep this directory at `hw5/.guides/tests`, alongside `hw5/.guides/support`.
Install a C++17 compiler, Make, CMake, Google Test, Valgrind, and Python 3.
No Python packages are needed.
On Ubuntu, install Google Test with `sudo apt-get install libgtest-dev`.
The suite builds that package's sources locally to keep headers and libraries
consistent, even when an incompatible prebuilt Google Test library is installed.
For a different source location, configure with `-DGTEST_SOURCE_DIR=<path>`.

From the workspace root, run `make smoke-test` to compile and validate one demo
scenario without profiling. Run `make test` to configure, build, and run all
tests. Use `make test TEST=13` to run a single numbered test. Performance
tests also run the included Starter and Instructor reference executables and
measure peak memory. Every `.cpp` file in `src/` is compiled.

The test build uses two compiler jobs. Reference memory profiles are reused
after the first successful measurement and regenerated when the reference
executable or profiling command changes. Reference runtimes and all student
measurements are run every time.

The tests build in `build/tests`, where `check-build.log`, `check.log`, and the
memory profiles in `massif/` are saved. `make clean` removes all build output,
including saved reference profiles.
