#include "common.h"
#include "stringbuf.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
sb_init(StringBuf *s)
{
	assert(s != NULL);
	s->data = malloc(128);
	s->capacity = 128;
	s->size = 0;
}

void
sb_check(StringBuf *s)
{
	assert(s != NULL);
	assert(s->capacity >= 0);
	assert(s->size >= 0);
	assert(s->capacity >= s->size);
}

void
sb_ensure_capacity(StringBuf *s, int spare)
{
	sb_check(s);
	while (s->capacity - s->size < spare) {
		s->capacity *= 2;
	}
	s->data = realloc(s->data, s->capacity);
}

int
sb_printf(StringBuf *s, const char *fmt, ...)
{
	va_list args;
	int printed;

	sb_check(s);

	va_start(args, fmt);
	sb_ensure_capacity(s, 16);
	
	printed = vsnprintf(s->data+s->size, s->capacity - s->size, fmt, args);
	if (printed >= s->capacity - s->size) {
		sb_ensure_capacity(s, printed);
		printed = vsnprintf(s->data+s->size, s->capacity - s->size, fmt, args);
	}
	s->size += printed;
	return printed;
}

unsigned
hexchar_val(char c)
{
	return c >= '0' && c <= '9' ? c - '0'
		 : c >= 'a' && c <= 'f' ? c - 'a' + 0x0A
		 : c - 'A' + 0x0A;
}

StringBuf
sb_from_hex(const char *hex)
{
	StringBuf s;
	size_t len = strlen(hex);

	sb_init(&s);
	for (size_t i = 0; i < len; i++) {
		unsigned int c1, c2;
		if (i+1 < len && (c1=hex[i], isxdigit(c1)) && (c2=hex[i+1], isxdigit(c2))) {
			unsigned c1d = hexchar_val(c1);
			unsigned c2d = hexchar_val(c2);
			sb_printf(&s, "%c", c1d*16 + c2d);
			i++; // we already processed the next char
		}
	}
	return s;
}

void
sb_print_hex(StringBuf *s)
{
	for (int i=0; i < s->size; i++) {
		printf("%02x", s->data[i]);
	}
	printf("\n");
}

StringBuf
sb_line_from_file(FILE *f)
{
	int ch;
	StringBuf s;

	sb_init(&s);

	while ( (ch = fgetc(f)) != EOF) {
		if (ch == '\n' || ch == '\r') break;
		sb_printf(&s, "%c", ch);
	}

	return s;
}

typedef struct {
	int valid_bits;
	int buf;
} base64_read_state;

static void
base64_read_init(base64_read_state *s)
{
	s->valid_bits = 0;
	s->buf = 0;
}

static bool
base64_read_update(base64_read_state *s, int ch, StringBuf *sb)
{
	int raw;
	if (ch >= 'A' && ch <= 'Z')
		raw = ch - 'A';
	else if (ch >= 'a' && ch <= 'z')
		raw = ch - 'a' + 26;
	else if (ch >= '0' && ch <= '9')
		raw = ch - '0' + 52;
	else if (ch == '+')
		raw = 62;
	else if (ch == '/')
		raw = 63;
	else if (isspace(ch) || ch == '=')
		return true;
	else {
		fprintf(stderr, "unrecognized base64 char '%c' (0x%02x)", ch, ch);
		return false;
	}
	s->buf = (s->buf << 6) | raw;
	s->valid_bits += 6;
	//printf("raw: %d, buf: 0x%0x, valid_bits==%d\n", raw, s->buf, s->valid_bits);
	if (s->valid_bits >= 8) {
		sb_printf(sb, "%c", s->buf >> (s->valid_bits - 8));
		s->valid_bits -= 8;
	}
	return true;
}

StringBuf
sb_from_base64_iostream(FILE *f)
{
	int ch;
	base64_read_state st;
	StringBuf sb;

	sb_init(&sb);

	base64_read_init(&st);
	while ((ch = fgetc(f)) != EOF) {
		base64_read_update(&st, ch, &sb);
	}

	return sb;
}

StringBuf
sb_from_base64_file(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		die("open '%s'", filename);

	StringBuf s = sb_from_base64_iostream(f);
	fclose(f);
	return s;
}

void
sb_hexdump(StringBuf *s, const char *title)
{
	if (title == NULL || !*title)
		title = "(unnamed string)";

	printf("%s (%d bytes):\n", title, s->size);
	for (int base=0; base < s->size; base += 16) {
		for (int i = 0; i < 16; i++) {
			if (base + i < s->size)
				printf("%02hhx ", s->data[base + i]);
			else
				printf("   ");
			if (i % 16 == 8)
				printf(" ");
		}
		printf("| ");
		for (int i = 0; i < 16; i++) {
			if (base + i < s->size) {
				char c = s->data[base + i];
				if (!isprint(c))
					c = '.';
				printf("%c", c);
			} else
				printf(" ");
		}
		printf("|\n");
	}
}
