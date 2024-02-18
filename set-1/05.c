#include <stdio.h>
#include <string.h>

void
print_xorred(const char *s, const char *passwd)
{
	size_t l = strlen(s);
	size_t passwd_len = strlen(passwd);
	for (int i=0; i < l; i++) {
		printf("%02x", s[i] ^ passwd[i % passwd_len]);
	}
	printf("\n");
}

int main(int argc, char **argv) {
	const char *s = "Burning 'em, if you ain't quick and nimble\n"
					"I go crazy when I hear a cymbal";
	const char *passwd = "ICE";
	if (argc >= 1+1)
		s = argv[1];
	if (argc >= 1+2)
		passwd = argv[2];

	printf("s: '%s', passwd: '%s'\n", s, passwd);

	print_xorred(s, passwd);
}
