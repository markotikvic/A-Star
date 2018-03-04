# make sure to use \t instead of spaces
# set the name of the project
PROJ_NAME = a-star

# compiler
CC = gcc

# compiler flags
CFLAGS = -Wall

# output object files directory
ODIR = obj

# external libraries directory
LDIR = ../lib

# library linkage (e.g. -lm for math.h)
LIBS =

# dependencies (e.g. util.h)
DEPS = a_star.h

# object files (e.g. main.o util.o from main.c and util.c)
# order of object files matter
_OBJ = a_star.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# compile to object files
$(ODIR)/%.o : %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

# link the object files in an executable
all : $(OBJ)
	$(CC) -o $(PROJ_NAME) $^ $(CFLAGS) $(LIBS)

.PHONY: clean

# clean up after you're done
clean :
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
