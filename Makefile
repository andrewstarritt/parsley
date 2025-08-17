# parsley make file
#

# Allows make to be run from the top level.
#
.PHONY : all install clean  uninstall help FORCE

# Currently only one sub-directory.
#
SUBDIRS = src test documentation

all install clean uninstall: $(SUBDIRS)

# Note the all, clean, uninstall targets are passed from command-line
# via $(MAKECMDGOALS), are handled by each sub-directory's Makefile
# targets.

$(SUBDIRS): FORCE
	$(MAKE) -C $@  $(MAKECMDGOALS) 

# Force targets.
#
FORCE:

# end
