.PHONY: export references

export: references
	@$(MAKE) -C hw5 clean
	@find .guides/secure -type d -name __pycache__ -prune -exec rm -rf {} +
	@rm -rf .guides/secure/.support
	@cp -a hw5/.support .guides/secure/.support
	@cp hw5/mothership.h .guides/secure/.support/mothership.h
	@python3 -B .dev/test_grading.py
	@python3 -B .dev/test_submission.py
	@printf 'Student project ready: %s\n' "$(CURDIR)/hw5"

references:
	@python3 -B .dev/references/build_portable.py
	@python3 -B .dev/references/profile_memory.py
