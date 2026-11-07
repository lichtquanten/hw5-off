Consider the popular game (at least at the time of this writing) of [Wordle](https://www.nytimes.com/games/wordle/index.html).  A 5-letter English word is chosen at random (though we will extend this to support `n`-letter words) and the user has some number of chances to guess the word.  As the user guesses words, letters in their guess that match the selected word in the **correct location** are highlighted in green, while letters (which may include duplicates) that are part of the word but **not in the correct location** are highlighted in yellow.  So, if the selected word was `total` and the user guessed `treat`, the first `t` and `a` in `treat` would be highlighted in *green* to indicate they are in the correct location and the last `t` would be marked in *yellow* to indicate that there is another `t` in the select word but not at the correct location, and all other letters (such as `r`) would appear in gray to indicate they are not found in the word.

In this program, you will write a tool to aide players of a **Wordle-like** game (it does not have the exact semantics of Wordle, but is similar).  Suppose the user would like to know what English-language words exist that meet certain criteria (so they can think of potential guesses).  The user will provide you an input of how many letters the word is, what letters have already been guessed in their correct location (akin to the green letters), and a list of letters that they know MUST appear in the word (akin to the yellow letters, but without indicating where those letters CANNOT be). They will NOT provide the list of gray letters, meaning you will need to try all possible letters when trying to fill in a blank location in the word.

So your task will be to write a function (and any desired helper functions), to compute all possible *English-language words* that exist given a string containing the already-guessed letters that are in the **correct location** (we will refer to these as **fixed** letters since they may not change position and must stay in that location) and a string of the already-guessed letters that are **MUST** appear in the word somewhere (we will refer to these as **floating** letters since their position may be in any of the remaining locations).  Again, we are **not actually writing code to play the game but code that can help players consider potential future guesses** given *some* of the knowledge they've learned from previous guesses. Your approach should be to generate all possible strings of lower-case, alpha characters that contain the fixed haracters (in their given location), floating characters (in any locations), and fill in any remaining letters with all possible options and then check if any of those strings are true *English-language* words.  We will provide a `std::set<std::string>` of all possible English-language words (i.e. essentially a dictionary) that you may use for this task (which we read from a provided file: `dict-eng.txt`).

The signature of the function you are to write must be (and cannot be changed):

```c++
std::set<std::string> wordle(
    const std::string& in,
    const std::string& floating,
    const std::set<std::string>& dict);
```

*Note*: We realize that this may be an inefficient approach to finding all the words that match the given constraints. If we were not forcing you to use this approach, it may be easier to find the words that start with a given letter and iterate through them in the sorted order that the set provides checking to see if the word matches the given constraints.  However, one can see that if the first letter or two are unknown, then this may require iterating through the whole dictionary, and so, in some cases, may be faster to generate all the possible strings, as we do here.

#### Input format

To indicate how many letters the selected word has **AND** the **correct, fixed location** letters already guessed, we will use a string of length `n` (for an `n`-letter word), using the `-` character to indicate a blank location and characters filling in the **correct, fixed locations**.  So the input string `-i-` means we want to find all **3 letter** words that have an `i` as the middle character.

We will then pass in a second string of the **floating** (yellow) characters that contains the characters (in any order) that we know must be part of the selected word but whose location is unknown.  So if you were given a first input string of `-i--` and second input string `dn`, you should output all words that have `i` as the 2nd letter and contain `d` and `n`.  A sample output is below. Since the results are stored in a set, they can be printed in any order.  We have provided a "driver"/test program (`wordle-driver.cpp`) that will take in these two strings from the command line, call your function, and then print out the results from the set of strings returned.

Program execution and command line arguments.

```bash
./wordle-driver -i-- dn
```

Printed results:

```
bind
dine
ding
dins
dint
find
hind
kind
mind
rind
wind
```

Use `wordle-driver` to do some sanity tests of your code before moving on to any of the tests from our grading suite.  Note:  To discourage any attempt to hard-code or game the system, the instructor may run additional tests after submission that were not provided, though they will be similar in format and content.  

#### Requirements and Assumptions

 - As always you may not change the signature of the primary function provided.
 - You MAY define helper functions in `wordle.cpp`.
 - You **MUST** use a recursive approach to find all combinations of letters to form the length-`n` word.  **Failure to do so will lead to a 0 on this part of the assignment.**  
    - Really you should only have 1 or 2 loops to help set the characters in any given location, and maybe 1 to 2 other loops to help with various constraint checks, etc.  But to ensure you do not somehow brute-force the solutions, you may use at most 5 loops in your entire implementation in `wordle.cpp`.
 - You may NOT use any functions from the `algorithm` library (nor should you really need to).
 - You do NOT need to implement any kind of *backtracking* approach where you attempt to determine if a string can possibly be an valid English-language word as you are forming it. Instead, just generate all possible strings and check if each word is in the dictionary once it has the full `n` letters.

#### Hints and Approach

Recall that when generating all combinations you use recursion to build up a combination 1 "place" at a time, with each recursion responsible for 1 "place/location" of the combination.  For that place, each recursion should try all the viable "options", recursing after each one, and, upon return, undo and try the next option if a solution has not been found (or if multiple solutions are desired).  Think carefully about what "options" should be tried at each location.  Can any letter be used at any open place?  What limitaiton do the **floating** letters provide?

By generating all combinations of `n`-length words *that meet the restrictions given by the **fixed** and **floating** characters*, it becomes simple to check whether the combination is a valid *English-language* word once the combination has its full `n` letters.