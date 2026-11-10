WORKSPACE := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

.PHONY: test references export-student

test:
	@python3 -m unittest discover -s "$(WORKSPACE)/.dev/runner_tests"

references:
	@python3 "$(WORKSPACE)/.dev/references/build_portable.py"

export-student:
	@python3 "$(WORKSPACE)/.dev/export_student.py"
