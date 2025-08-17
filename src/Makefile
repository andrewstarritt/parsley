# Makefile  src/parsley
#

.PHONY: all install clean uninstall always

TOP=..

# was -std=gnu++11 

OPTIONS += -Wall -pipe -O3 -g -std=gnu++11 -m64 -fPIC

COPTS   = $(OPTIONS)
COPTS  += -DBUILDING_PARSLEY_LIBRARY

OBJECTS += parsley.o

LIBRARY = libparsley.so

INSTALL_LIB_DIR   = /usr/local/lib
INSTALL_LIB       = $(INSTALL_LIB_DIR)/$(LIBRARY)

INSTALL_INC_DIR   = /usr/local/include
INSTALL_INCLUDES += $(INSTALL_INC_DIR)/parsley.h

all : $(LIBRARY)  Makefile

install: $(INSTALL_LIB) $(INSTALL_INCLUDES)  Makefile

$(INSTALL_LIB): $(LIBRARY)  Makefile
	@echo "\033[33;1minstalling\033[00m $@"  && \
	sudo cp $(LIBRARY)  /usr/local/lib/

$(LIBRARY) : $(OBJECTS)   Makefile
	g++ $(OPTIONS) -o $(LIBRARY) -shared $(OBJECTS)  $(LINK_OPTS)
	@echo ""

parsley.o : parsley.h  parsley.cpp  Makefile
	g++ $(COPTS) -c parsley.cpp


# $< is source file, $@ is target file, % is wild card
#
$(INSTALL_INC_DIR)/%.h : %.h   Makefile
	@echo "\033[33;1minstalling\033[00m $@"
	@sudo cp $<  $@

clean:
	rm -f *.o *.so

uninstall:
	@echo "uninstalling $(INSTALL_LIB) and library header files."
	@rm -f  $(INSTALL_LIB)  $(INSTALL_INCLUDES) || echo "    run make uninstall as user root."
# end
