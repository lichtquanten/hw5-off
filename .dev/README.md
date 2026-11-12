# Preparing the assignment

Run `make export` from the outer workspace. This builds and verifies the reference
binaries, cleans `hw5/`, refreshes `.guides/secure/.support/`, and checks grading.
Edit `hw5/.support/`; the secure snapshot is generated, not maintained separately.
The snapshot also contains the provided `hw5/mothership.h` API header, which the
grader restores alongside the course tests and simulator.
Publish the refreshed `.guides/secure/` folder along with the student project.

Students use `make`, `make smoke-test`, and `make test` inside `hw5/`. That folder
must be its own Git repository with `origin` pointing to their repository.
They must push the commit before submitting its SHA.

## Codio SHA assessment

Keep the SHA input assessment named **Final SHA**. For the old workflow, use a
zero-point Free Text Autograde assessment with `/bin/true` as its command to
record the answer. Enable **Script Grading** and set its command to
`python3 .guides/secure/grade.py`. Disable Assessments Grading for the assignment.
The grader reads `assessments.info` from `CODIO_AUTOGRADE_ENV`, selects **Final
SHA**, and expects its saved text in `answer.correctAnswer`. Codio documents the
assessment envelope and uses this field in its examples, but does not show a
free-text response example. Confirm this field and private-repository access
with **Test Autograde Script** before publishing. An unexpected response format
stops grading without reporting a score; it never falls back to workspace code.

Alternatively, to grade immediately when students submit their SHA, set that
Free Text Autograde assessment's command to `python3 .guides/secure/grade.py`,
timeout to 180 seconds, points to 100, partial points on, and maximum score off.
Use Assessments Grading instead of Script Grading in that setup. Codio supplies
`CODIO_FREE_TEXT_ANSWER` directly. The entry point chooses `send_grade_v2` for
assignment grading and `send_partial_v2` for a partial-points assessment.

The grader clones `hw5/`'s origin into a temporary directory, checks out the SHA,
replaces `.support/` with the protected snapshot, and uses the course's CMake
files directly. It never runs the submitted Makefile. Feedback includes the
resolved commit SHA. Missing or unavailable SHAs receive zero with an explanation;
grading infrastructure errors stop without reporting a score.

The grading command takes no arguments. Codio supplies the submitted SHA through
its environment. Local integration tests simulate that environment and the Codio
grade API without sending real grades.

Codio references: [assignment-level grading](https://docs.codio.com/instructors/authoring/assessments/auto-grade-scripts.html),
[free-text grading](https://docs.codio.com/instructors/authoring/assessments/autograde-free-text.html),
and [secure grading files](https://docs.codio.com/instructors/authoring/assessments/assessment-security.html).
