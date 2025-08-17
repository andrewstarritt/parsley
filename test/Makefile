# test Makefile
#
TOP=$(shell readlink -f .. )

OPTIONS += -Wall -pipe -O3 -g -std=gnu++11 -m64 -fPIC

# We use the local include rather than the installed items
#
COPTS += -I. -I$(TOP)/src
LOPTS += -L$(TOP)/src -Wl,-rpath,$(TOP)/src -lparsley

.PHONY : all install  clean uninstall  FORCE

all : parsley_test  parsley_exp Makefile

install : parsley_test  parsley_exp Makefile

parsley_test :  parsley_test.o  Makefile
	g++ $(OPTIONS) -o  parsley_test parsley_test.o  $(LOPTS)

parsley_test.o: parsley_test.cpp $(TOP)/src/parsley.h  Makefile
	g++ $(OPTIONS) -c $(COPTS) -o parsley_test.o parsley_test.cpp

parsley_exp :  parsley_exp.o  Makefile
	g++ $(OPTIONS) -o  parsley_exp parsley_exp.o  $(LOPTS)

parsley_exp.o: parsley_exp.cpp $(TOP)/src/parsley.h  Makefile
	g++ $(OPTIONS) -c $(COPTS) -o parsley_exp.o parsley_exp.cpp

clean :
	rm -f *.o  parsley_test  parsley_exp

uninstall :
	@:

# end
