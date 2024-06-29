###############################################################################
#
#                               Makefile
#
#     Assignment: um
#     Authors:    Marten Tropp and Matthew Carey
#     Date:       11/20/2023
#      
#     The purpose of this file is to compile and link all the necessary files 
#     for the um program.
#
#    
#
###############################################################################

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I. -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
CFLAGS = -g -std=c99 -O2 -Wall -Wextra -Werror -Wfatal-errors -pedantic \
         $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
LDLIBS = -lcii40-O2 -l40locality -larith40  -lnetpbm -lcii40 -lm -lrt

# Collect all .h files in our directory.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: um

## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
# %.o: %.c $(INCLUDES)
# 	$(CC) $(CFLAGS) -c $< -o $@

run_um.o: run_um.c
	$(CC) $(CFLAGS) -c $< -o $@

## Linking step (.o -> executable program)

um: run_um.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f um *.o

