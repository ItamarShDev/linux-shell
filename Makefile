linux-shell
===========

OS Course  Linux Shell Immitation


#
# Created By ITAMAR SHARIFY
#
# our compiler
CC=gcc

# compile arguments
CFLAGS+=-c -g -Wall

# linker flags
LDFLAGS+=


# our source files
SOURCES= Shell.c
# a macro to define the objects from sources
OBJECTS=$(SOURCES:.c=.o)

# executable name	
EXECUTABLE=Shell

$(EXECUTABLE): $(OBJECTS)
	@echo "\n Building target" $@ "..."
	@ $(CC) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "***************************************"
	@echo "*             Welcome!                *"
	@echo "* -------------menu:------------------*"
	@echo "*    type 'make run' to run the app   *"
	@echo "*      type 'make clean' to delete    *"
	@echo "*           all created files         *"
	@echo "*                                     *"
	@echo "*                ENJOY                *"
	@echo "***************************************"
# a rule for generating object files given their c files
.c.o:
	@echo "*   Compiling" $< "...   "
	@ $(CC) $(CFLAGS) $< -o $@
clean:
	@echo "Cleaning..."
	@ rm -rf *s *o $(EXECUTABLE)

.PHONY: all clean
run:
	@ ./Shell
