#include "common.h"
#include "stringbuf.h"

#include <string.h>

void test_sb_from_base64_iostream(const char *b64, const char *expected_data) {
	struct timespec start = timespec_now();
	
	FILE *f = tmpfile();

	if (!f)
		die("tmpfile");

	size_t len = strlen(b64);
	if (len > 0) {
		size_t res;
		res = fwrite(b64, len, 1, f);
		if (res != 1)
			die("fwrite");
		rewind(f);
	}
	
	StringBuf s = sb_from_base64_iostream(f);
	bool ok = (strcmp(s.data, expected_data) == 0);
	printok(ok, &start, "base64_iostream('%s')", b64);
	if (!ok) {
		printf("got      '%s' (0x", s.data);
		for (size_t i = 0; i < s.size; i++) {
			printf("%02hhx", s.data[i]);
		}
		printf(")\n");
		printf("expected '%s' (0x", expected_data);
		for (size_t i = 0; i < s.size; i++) {
			printf("%02hhx", expected_data[i]);
		}
		printf(")\n");
	}
}

int main(int argc, char **argv)
{
	test_sb_from_base64_iostream("", "");
	// 25,32 => 0x66
	// 011001 100000 => 0110 0110
	test_sb_from_base64_iostream("Zg==", "f");
	test_sb_from_base64_iostream("Zm9vYmE=", "fooba");
}
