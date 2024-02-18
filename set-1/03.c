#include "stringbuf.h"

#include <ctype.h>
#include <stdio.h>

int main() {
	StringBuf s = sb_from_hex("1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736");
	int max_printable = -1;
	int optimal_code = 0;

	for (int code=0; code<255; code++) {
		StringBuf n;
		int n_printable = 0;
		sb_init(&n);

	for (int i=0; i < s.size; i++) {
	unsigned char newchar = s.data[i] ^ code;
			sb_printf(&n, "%c", newchar);
			n_printable += isalpha(newchar);
		}

		if (n_printable > max_printable) {
			max_printable = n_printable;
			optimal_code = code;
		}
	}

	for (int i=0; i < s.size; i++) {
		unsigned char newchar = s.data[i] ^ optimal_code;
		printf("%c", newchar);
	}
	printf("\ncode:%d\n", optimal_code);
}
