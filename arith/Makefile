# Add header


############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I. -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
LDLIBS = -l40locality -larith40 -lnetpbm -lcii40 -lm -lrt

# Collect all .h files in our directory.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: 40image ppmdiff


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

ppmdiff: ppmdiff.o uarray2b.o uarray2.o a2plain.o a2blocked.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

40image: 40image.o compress40.o bitpack.o uarray2b.o uarray2.o a2plain.o \
	    a2blocked.o our_math.o file_IO.o rgb_to_yPbPr.o yPbPr_to_rgb.o  \
	    yPbPr_to_word.o word_to_yPbPr.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f 40image ppmdiff *.o
