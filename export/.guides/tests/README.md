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
tests also run the included Instructor and Round Robin reference executables and
measure peak memory. Every `.cpp` file in `src/` is compiled.

The test build uses two compiler jobs. Reference memory profiles are reused
after the first successful measurement and regenerated when the reference
executable or profiling command changes. Reference runtimes and all student
measurements are run every time.

### Performance Checks and Grading

The per-test limits are in `runner/catalog.py`: `max_ticks`,
`max_runtime_seconds`, and `max_memory_bytes`. Limits use strict `<` comparisons
against the original measurements; the displayed numbers are rounded. An unset
limit is not checked. Test 18 has no tick limit.

Tests 1–12 earn one credit each for correctness. Tests 13–18 earn one credit
each when correctness and every configured performance check pass. Both
`make test` and `.guides/secure/grade.py` use the same calculation: credits
divided by 18. A single-test run reports that test's credit without displaying
an overall assignment score. `make test` exits unsuccessfully if any selected
test does not earn credit.

The performance tables include measurements for all three strategies and
ratios of Yours to Instructor and Round Robin. Runtime is measured outside
Valgrind. Peak memory is the maximum Massif heap measurement for the whole
test process, including the simulator. A missing student memory measurement
fails the memory check and is reported with its profiling error.

Each of tests 13–17 totals eight fixed scenarios: four robot indices crossed
with two wreck depths, using 250,000-segment paths. Every scenario must be
correct. Ticks and runtime cover the whole group; peak memory is the maximum
during that group. Maps are generated and released one at a time. Test 18
remains a single full-size, unfindable scenario for resource stress.

The wreck's debris field spans a broad region across its path and its two
neighbors, with density increasing toward the wreck's depth. Displaced fields
are narrower; the misleading case makes those distractions denser. Weak and
sparse cases reduce the wreck field's density. Signal strength still counts
each nearby discovered debris segment equally.

Seed or distribution changes require rebuilding both reference executables
and reviewing the limits. The limits are rounded per-case budgets, not a
uniform percentage of either reference. Round Robin explores paths in equal
turns without using debris; Instructor uses signal strength to guide its search.

The tests build in `build/tests`, where `check-build.log`, `check.log`, and the
memory profiles in `massif/` are saved. `make clean` removes all build output,
including saved reference profiles.
