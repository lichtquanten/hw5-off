## Grading
There are 26 tests between `wordle_tests` and `schedwork_tests`. No valgrind tests this time (you don't need to dynamically allocate anything on this assignment).

## Push your commits to GitHub
Push your source code and all relevant files. Do not push binary files or “garbage” files. (Use the `.gitignore` file to make this as easy as possible on yourself as well as `make clean` to delete any object files or executables).

You should NOT add/commit/push any of the test suite folders that may be released before the submission deadline (e.g. `hw5_tests`). This can easily happen if you use `git add .`. Instead, we recommend adding specific files (e.g. `git add file1.h file1.cpp file2.h file2.cpp Makefile`) or using specific wildcards: `git add *.h *.cpp Makefile` (which will leave out anything in a subfolder).

If you do happen to add/commit/push a test suite, you can remove it by running:
```
git rm -rf hw5_tests
git commit -m "Removed tests"
git push
```

<input type="checkbox"> <b>Run `git status` on your repository</b> and make sure that there are no files listed as:
- Changes to be committed,
- Changes not staged for commit, or
- Untracked files


A git status on your master branch must return:
```markdown-hide-clipboard
# On branch master
nothing to commit, working directory clean
```

If files that you do not want to push (i.e. `hw5_tests` or other object files) appear in the untracked list, add that folder/filename(s) to your `.gitignore`. (Note: you must add/commit/push your `.gitignore` like any other file).

Make sure you have pushed your desired code to Github and then use `git log` to get the SHA. Paste the SHA in the box below to submit.

{Check It!|assessment}(free-text-auto-2341275755)
