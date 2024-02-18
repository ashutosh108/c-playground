CFLAGS=-Wall -O0 -Werror -ggdb3
PREFIX=/tmp/cryptopals
EXES=01 02 03 04 05 06 stringbuf.test
EXES_TMP=$(foreach e,$(EXES),${PREFIX}/$(e))
all: prefix_dir | ${EXES_TMP}

prefix_dir:
	@mkdir -p ${PREFIX}

.PHONY: prefix_dir

${PREFIX}/%.o: %.c
	${CC} ${CFLAGS} ${CPPFLAGS} -c $< -o $@

${PREFIX}/03: ${PREFIX}/03.o ${PREFIX}/stringbuf.o
${PREFIX}/04: ${PREFIX}/04.o ${PREFIX}/stringbuf.o
${PREFIX}/stringbuf.test: ${PREFIX}/stringbuf.test.o ${PREFIX}/stringbuf.o
${PREFIX}/06: ${PREFIX}/06.o ${PREFIX}/stringbuf.o

clean:
	rm -fr *.o ${PREFIX}

06-run: ${PREFIX}/06
	${PREFIX}/06
