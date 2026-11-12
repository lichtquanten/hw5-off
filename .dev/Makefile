WORKSPACE := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

.PHONY: references reference-memory-profiles export-student

references:
	@python3 "$(WORKSPACE)/.dev/references/build_portable.py"
	@$(MAKE) -f "$(WORKSPACE)/.dev/Makefile" reference-memory-profiles

reference-memory-profiles:
	@python3 "$(WORKSPACE)/.dev/references/profile_memory.py"

export-student: references
	@python3 "$(WORKSPACE)/.dev/test_grading.py"
	@python3 "$(WORKSPACE)/.dev/export_student.py"
