#include "stringbuf.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *f = fopen("4.txt", "r");
	if (!f) {
		perror("fopen 4.txt");
		exit(EXIT_FAILURE);
	}
	int linenum = 0;

	while (!feof(f)) {
		StringBuf hex = sb_line_from_file(f);
		StringBuf s;
		if (ferror(f)) {
			perror("reading file");
			exit(EXIT_FAILURE);
		}

		linenum++;

		//printf("%s\n", s.data);
		s = sb_from_hex(hex.data);

		int max_printable = INT_MIN;
		int optimal_code = 0;

		for (int code=0; code<255; code++) {
			StringBuf n;
			int n_printable = 0;
			sb_init(&n);

			for (int i=0; i < s.size; i++) {
				unsigned char newchar = s.data[i] ^ code;
				sb_printf(&n, "%c", newchar);
				n_printable += 3*!!isalpha(newchar);
				n_printable -= !isprint(newchar);
			}

			if (n_printable > max_printable) {
				max_printable = n_printable;
				//printf("code: %d->%d\n", optimal_code, code);
				optimal_code = code;
			}
		}

		printf("%s: line %d, code:%d\n", hex.data, linenum, optimal_code);
		for (int i=0; i < s.size; i++) {
			unsigned char newchar = s.data[i] ^ optimal_code;
			if (isprint(newchar)) {
				printf("%c", newchar);
			} else {
				printf("<%02x>", newchar);
			}
		}
		printf("\n");
	}
}
