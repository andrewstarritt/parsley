# Makefile
#

TOP=..

#
# Note: we do not include the rule $(TOP)/configure/RULES

# Define targets
#
T1 = $(TOP)/html/index.html

.PHONY: all install clean

all: $(T1)

install:  all

# NOTE: a failure of any of these doco build commands does NOT cause an overall make failure.
#
$(T1): parsley.cfg  mainpage.dox ../src/*h ../src/*cpp
	@doxygen  parsley.cfg  || echo "*** doxygen not available or failed."

clean:

uninstall:

# end
