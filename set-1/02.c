#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *buf;
	int size;
	int capacity;
} StringBuf;

void
sb_init(StringBuf *s)
{
	assert(s != NULL);
	s->buf = malloc(128);
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
	s->buf = realloc(s->buf, s->capacity);
}

int
sb_printf(StringBuf *s, const char *fmt, ...)
{
	va_list args;
	int printed;

	sb_check(s);

	va_start(args, fmt);
	sb_ensure_capacity(s, 16);
	
	printed = vsnprintf(s->buf+s->size, s->capacity - s->size, fmt, args);
	if (printed >= s->capacity - s->size) {
		sb_ensure_capacity(s, printed);
		printed = vsnprintf(s->buf+s->size, s->capacity - s->size, fmt, args);
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
		printf("%02x", s->buf[i]);
	}
	printf("\n");
}
int main()
{
	StringBuf s = sb_from_hex("1c0111001f010100061a024b53535009181c");

	sb_print_hex(&s);

	StringBuf code = sb_from_hex("686974207468652062756c6c277320657965");

	sb_print_hex(&code);

	StringBuf r;
	sb_init(&r);
	sb_ensure_capacity(&r, s.size+1);

	for (int i=0; i < s.size; i++) {
		r.buf[i] = s.buf[i] ^ code.buf[i];
	}
	r.size = s.size;
	
	sb_print_hex(&r);
}
