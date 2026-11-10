# CS104 HW5 Test Suite

The Python test runner and Codio grader support Python 3.6.9 and newer using
only the Python standard library. Run the Codio autograder with
`python3 .guides/secure/grade.py` from the workspace root; add `--dry-run` to
run it without submitting a grade.

### Using the Tests
1. Keep this directory at `hw5/.guides/tests`, alongside `hw5/.guides/support`
2. Open a terminal and cd into `.guides/tests`
3. Install the Python dependencies: `sudo apt-get install python3-xmltodict`
3. Initialize the test suite by running `cmake .`
4. Run the tests by typing `make grade`.  To run the tests again, you only need to execute this command, not any others.

### Grade Output
Running `make grade` will generate a couple of outputs:
1. In the root directory: A formatted Markdown grade report, ready for submission to GitHub
2. In the `compile-logs` directory: Logs of the compilation output for each problem
3. In the `results` directory: Files listing the results of each test in a standard format, for input to other grading systems
4. In the `test-output` directory: Files containing the output each test printed.  To figure out why a test failed, start here!
