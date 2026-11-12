WORKSPACE := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
.DEFAULT_GOAL := export

.PHONY: export references reference-memory-profiles

export: references
	@$(MAKE) -C "$(WORKSPACE)/hw5" clean
	@find "$(WORKSPACE)/.guides/secure" -type d -name __pycache__ -prune -exec rm -rf {} +
	@rm -rf "$(WORKSPACE)/.guides/secure/.support"
	@cp -a "$(WORKSPACE)/hw5/.support" "$(WORKSPACE)/.guides/secure/.support"
	@cp "$(WORKSPACE)/hw5/mothership.h" "$(WORKSPACE)/.guides/secure/.support/mothership.h"
	@python3 -B "$(WORKSPACE)/.dev/test_grading.py"
	@python3 -B "$(WORKSPACE)/.dev/test_submission.py"
	@printf 'Student project ready: %s\n' "$(WORKSPACE)/hw5"

references:
	@python3 "$(WORKSPACE)/.dev/references/build_portable.py"
	@$(MAKE) -f "$(WORKSPACE)/Makefile" reference-memory-profiles

reference-memory-profiles:
	@python3 "$(WORKSPACE)/.dev/references/profile_memory.py"
