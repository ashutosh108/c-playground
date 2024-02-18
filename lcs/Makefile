CFLAGS ?= -Wall -Werror -ggdb3
CPPFLAGS ?= -I../set-1

all: lcs.test

test: lcs.test lcs-diff
	./lcs.test
	./lcs-diff file1 file2
	

lcs.test: lcs.test.o lcs.o

lcs-diff: lcs-diff.o lcs.o

clean:
	rm -f lcs-diff lcs.test *.o
