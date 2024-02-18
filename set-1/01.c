#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug = 0;
int run_test = 0;

void parse_options(int argc, char **argv) {
	while (1) {
		int c = getopt_long(argc, argv, "dt", NULL, NULL);
		if (c == -1) break;
		switch (c) {
		case -1: break;
		case 'd': debug++; break;
		case 't': run_test = 1; break;
		default:
			fprintf(stderr, "internal error: getopt_long returned unknown option '%c' (%d)\n", c, c);
			exit(EXIT_FAILURE);
		}
	}
}

unsigned int char_hex2dec(char c) {
	return c >= '0' && c <= '9' ? c - '0'
		:  c >= 'a' && c <= 'f' ? c - 'a' + 10
		:  c - 'A' + 10;
}

static char enc[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', // 0-7
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 8-15
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', // 16-23
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', // 24-31
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', // 32-39
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',	// 40-47
	'w', 'x', 'y', 'z', '0', '1', '2', '3', // 48-55
	'4', '5', '6', '7', '8', '9', '+', '/', // 56-63
};
// (pad) =

void flush_buf(unsigned char *buf, int n, FILE *out) {
	unsigned int acc = 0;
	int accbits = 0;
	for (int i=0; i < n; i++) {
		if (debug >= 1)
			printf("buf[%d] = '%02x'\n", i, buf[i]);
		acc  = ( acc << 8 ) | buf[i];
		accbits += 8;
		while (accbits >= 6) {
			unsigned code = ( acc >> (accbits-6) ) & 63;
			if (debug >= 1)
				printf("code = %02x (acc %x, accbits %d)\n", code, acc, accbits);
			acc &= ~( 63 << (accbits-6) );
			accbits -= (accbits < 6 ? accbits : 6);
			fprintf(out, "%c", enc[code]);
		}
	}
	// ...6...6
	// 01100110
	//
	if (accbits > 0) {
		unsigned code = acc << (6 - accbits);
		if (debug >= 1)
			printf("(flushing) code = %02x (acc %x, accbits %d)\n", code, acc, accbits);
		fprintf(out, "%c", enc[code]);
	}
	if (n == 1)
		fprintf(out, "==");
	else if (n == 2)
		fprintf(out, "=");
}

void hex2base64(const unsigned char *inbuf, size_t n, FILE *out) {
	unsigned char outbuf[3];
	int outbuf_ind = 0;
	for (int i=0; i < n;) {
		if (!isxdigit(inbuf[i])) {
			// pass non-hex chars as they are (presumably only whitespace)
			size_t nwritten = fwrite(&inbuf[i], 1, 1, out);
			if (nwritten != 1) {
				perror("fwrite");
				exit(EXIT_FAILURE);
			}
			i++;
			continue;
		}
		int c1, c2;
		if (i+1 < n && (c1=inbuf[i], isxdigit(c1)) && (c2=inbuf[i+1], isxdigit(c2))) {
			assert(outbuf_ind < sizeof outbuf);
			outbuf[outbuf_ind++] = char_hex2dec(c1) * 16 + char_hex2dec(c2);
			if (outbuf_ind >= 3) {
				flush_buf(outbuf, outbuf_ind, out);
				outbuf_ind = 0;
			}
			i += 2;
			continue;
		}
		fprintf(stderr, "can't parse hex: '%c' (+%ld more chars)\n", inbuf[i], n-i-1);
		i++;
	}
	if (outbuf_ind > 0) {
		flush_buf(outbuf, outbuf_ind, out);
	}
}

void hex2base64_file(FILE *in, FILE *out) {
	static const int BUF_SIZE = 1024;
	unsigned char buf[BUF_SIZE];
	size_t r;

	while ((r = fread(buf, 1, sizeof buf, in)) != 0) {
		hex2base64(buf, r, out);
	}
}

void hex2base64_is(const char *in, const char *expected_out) {
	FILE *f = fopen("/dev/null", "w");
	char hexbuf[1024];
	char buf[1024] = {};
	bool fail;
	buf[0] = '\0';
	setbuffer(f, buf, 1024);
	hexbuf[0] = '\0';
	for (size_t i=0; i < strlen(in); i++) {
		snprintf(hexbuf+2*i, (sizeof hexbuf)-2*i, "%02x", in[i]);
	}
	if (debug >= 1)
		printf("hex: '%s'\n", hexbuf);
	hex2base64((unsigned char *)hexbuf, strlen(hexbuf), f);
	fail = strcmp(expected_out, buf) != 0;
	printf("%s '%s'=>'%s'\n", fail ? "not ok" : "ok", in, expected_out);
	if (fail) {
		printf("expected: '%s'\n", expected_out);
		printf("got     : '%s'\n", buf);
	}
	fclose(f);
}

void test() {
	hex2base64_is("", "");
	hex2base64_is("f", "Zg==");
	hex2base64_is("fo", "Zm8=");
	hex2base64_is("foo", "Zm9v");
	hex2base64_is("foob", "Zm9vYg==");
	hex2base64_is("fooba", "Zm9vYmE=");
	hex2base64_is("foobar", "Zm9vYmFy");
}

int main(int argc, char **argv) {
	parse_options(argc, argv);
	if (run_test) {
		printf("Running self-test\n");
		test();
		exit(EXIT_SUCCESS);
	}
	if (optind < argc) {
		for (int arg = optind; arg < argc; arg++) {
			FILE *f = fopen(argv[arg], "rb");
			if (f == NULL) {
				fprintf(stderr, "fopen(%s): %m\n", argv[arg]);
				exit(EXIT_FAILURE);
			}
			hex2base64_file(f, stdout);
			if (fclose(f) != 0) {
				fprintf(stderr, "fclose(%s): %m\n", argv[arg]);
				exit(EXIT_FAILURE);
			}
		}
	} else {
		hex2base64_file(stdin, stdout);
	}
}
