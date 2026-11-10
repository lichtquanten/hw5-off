WORKSPACE := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

.PHONY: references export-student

references:
	@python3 "$(WORKSPACE)/.dev/references/build_portable.py"

export-student:
	@python3 "$(WORKSPACE)/.dev/export_student.py"
