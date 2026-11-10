# CS104 HW5 Test Suite

The test suite supports CMake/CTest 3.10 and newer, including Codio's 3.10.2
and the student image's 3.16.3. The Python test runner and Codio grader support
Python 3.6.9 and newer using only the Python standard library. Run the Codio autograder with
`python3 .guides/secure/grade.py` from the workspace root; add `--dry-run` to
run it without submitting a grade.

### Using the Tests

Keep this directory at `hw5/.guides/tests`, alongside `hw5/.guides/support`.
Install a C++17 compiler, Make, CMake, Google Test, Valgrind, and Python 3.
No Python packages are needed.

From the workspace root, run `make check` to configure, build, and run all
18 tests. Use `make check TEST=13` to run a single numbered test. Performance
tests also run the Starter and Instructor references and measure peak memory.
Use `make references` after changing the simulator or performance scenarios.

From this directory, `cmake .` followed by `make check` or `make grade` runs
the same runner. Build and test logs are saved as `check-build.log` and
`check.log` in the build directory; `check-results.xml` contains CTest's
dashboard XML results. Reference results and memory profiles are saved in
`reference-results/` and `massif/`.

Run runner regression checks with
`python3 -m unittest discover -s .guides/tests/runner_tests` from the workspace root.
