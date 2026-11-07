### There are two steps on this page: Github setup and testing (cmake) initialization.
### Do the following Github setup before writing any code or solving any written problems!

### Initialize your local repository and connect it to Github
1. Create a repo on [Github](https://github.com) named `hw5`. Do not add a `README.md` or `.gitignore`
1. The image below shows how to find the "SSH URL" used in the `git remote` command
![Image showing how to copy an ssh](.guides/img/copyssh.png)
Note: you **must** use the SSH URL! If you use the https URL you will recieve a grading penalty up to the full credit of the assignment!
1. Open a Codio terminal and change to the "hw5" directory: `cd hw5`
1. Verify your directory: `pwd` should output `/home/codio/workspace/hw5`
1. Complete the following on this Codio terminal.
```shell
git init
git add *.cpp *.h Makefile .gitignore hw5_tests.tar.gz
git commit -m "Initial commit for hw5"
git branch -M main
git remote add origin git@github.com:GHUSERNAME/hw5.git
git push --set-upstream origin main
```

|||important
## Notes:

You may need to type `yes` at the prompt to complete the task.
You will only need `--set-upstream origin main` the first time. Subsequent pushes will only need `git push`
|||

If the above was successful, you should see something like:
```shell
Counting objects: 4, done.
Delta compression using up to 8 threads.
Compressing objects: 100% (4/4), done.
Writing objects: 100% (4/4), 861 bytes | 861.00 KiB/s, done.
Total 4 (delta 0), reused 0 (delta 0)
To github.com:/tommyt/hw5.git
 * [new branch]      main -> main
Branch 'main' set up to track remote branch 'main' from 'origin'.
```

{Check It!|assessment}(test-1674751144)


### Now we need to initialize our testing setup
1. Open a Codio terminal and change to the "hw5" directory: `cd hw4`
1. Verify your directory: `pwd` should output `/home/codio/workspace/hw5`
1. Run the following commands to initialize the testing environment:
```shell
cd hw5_tests
cmake .
```
The last few lines of output from this command should be:
```shell
-- Found 15 GTests from wordle_tests
-- Found 11 GTests from schedwork_tests
-- Configuring done
-- Generating done
-- Build files have been written to: /home/codio/workspace/hw5/hw5_tests
```
### Note the testing environment is Linux only, so it will only work in Codio, Docker or a stand-alone Linux environment (not MacOS).

## Do not proceed until both of these steps work!