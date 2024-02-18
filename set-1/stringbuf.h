#ifndef STRINGBUF_H
#define STRINGBUF_H

#include <stdio.h>

typedef struct {
	char *data;
	int size;
	int capacity;
} StringBuf;

void sb_init(StringBuf *s);

void sb_check(StringBuf *s);

void sb_ensure_capacity(StringBuf *s, int spare);

int sb_printf(StringBuf *s, const char *fmt, ...);

StringBuf sb_from_hex(const char *hex);
StringBuf sb_from_base64_iostream(FILE *f);
StringBuf sb_from_base64_file(const char *filename);

void sb_print_hex(StringBuf *s);
void sb_hexdump(StringBuf *s, const char *title);

StringBuf sb_line_from_file(FILE *f);
#endif		/* STRINGBUF_H */
