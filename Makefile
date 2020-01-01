# Register all subdirectories in the project's root directory.
SUBDIRS := getcaldata tekfwtool tektool

default: all

# Top-level phony targets.
all clean: $(SUBDIRS) FORCE

# Recurse `make` into each subdirectory.
$(SUBDIRS): FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

# A target without prerequisites and a recipe, and there is no file named `FORCE`.
# `make` will always run this and any other target that depends on it.
FORCE:
