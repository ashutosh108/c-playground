#include "common.h"
#include "stringbuf.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>

int
bitcount(unsigned char n)
{
	int count = 0;
	while (n) {
		count++;
		n &= (n-1);
	}
	return count;
}

int
hamming_dist(const char *s1, int s1_len, const char *s2, int s2_len)
{
	int dist = 0;
	for (int i=0; i<s1_len; i++) {
		dist += bitcount(s1[i] ^ s2[i]);
	}
	return dist;
}

typedef struct {
	int keysize;
	double dist;
} pair;

static int
pair_compare(const void *a, const void *b)
{
	const pair *p1 = (pair *)a;
	const pair *p2 = (pair *)b;
	if (p1->dist < p2->dist)
		return -1;
	else if (p1->dist > p2->dist)
		return +1;
	return 0;
}

int
guess_keysize(const char *s, int s_len)
{
	const int MIN_COUNT = 3;
	const int MIN_KEYSIZE = 2;
	const int MAX_KEYSIZE = 40;
	pair mins[MIN_COUNT+1];
	
	for (int i = 0; i < MIN_COUNT; i++) {
		mins[i].keysize = 0;
		mins[i].dist = INFINITY;
	}

	for (int keysize = MIN_KEYSIZE; keysize <= MAX_KEYSIZE; keysize++) {
		if (s_len < keysize*6) {
			die("apptempted keysize %d is too large for s_len %d", keysize, s_len);
		}
		double dist = ((double)(
			hamming_dist(s, keysize, s+keysize, keysize)
		  + hamming_dist(s+2*keysize, keysize, s+3*keysize, keysize)
		  //+ hamming_dist(s+4*keysize, keysize, s+5*keysize, keysize)
		)) / (2 * keysize);
		mins[MIN_COUNT].keysize = keysize;
		mins[MIN_COUNT].dist = dist;
		qsort(&mins, MIN_COUNT+1, sizeof(pair), pair_compare);
		printf("keysize %d: dist=%f\n", keysize, dist);
	}
	for (int i=0; i < MIN_COUNT; i++) {
		printf("min_dist[%d]=%f, keysize=%d\n", i, mins[i].dist, mins[i].keysize);
	}
	return mins[1].keysize;
}

static int
break_xor_singlebyte(const char *s, int s_len)
{
	int max_score = INT_MIN;
	int optimal_code = 0;

	StringBuf decoded;
	sb_init(&decoded);

	for (int code=0; code <= 255; code++) {
		int score = 0;

		for (int i=0; i < s_len; i++) {
			unsigned char newchar = s[i] ^ code;
			score += 3*!!isalpha(newchar) - 1*(!isprint(newchar)) - 1*(iscntrl(newchar) && !isspace(newchar));
		}

		if (score > max_score) {
			max_score = score;
			//printf("code: %d->%d\n", optimal_code, code);
			optimal_code = code;
		}
	}

	for (int i=0; i < s_len; i++)
		sb_printf(&decoded, "%c", s[i] ^ optimal_code);
	printf("code %d, decoded='%s'\n", optimal_code, decoded.data);
	return optimal_code;
}

void
break_xor_for_keysize(const char *s, int s_len, int keysize)
{
	unsigned char key[keysize];
	for (int key_idx = 0; key_idx < keysize; key_idx++) {
		StringBuf tmp;
		sb_init(&tmp);
		for (int i = key_idx; i < s_len; i += keysize)
			sb_printf(&tmp, "%c", s[i]);
		key[key_idx] = break_xor_singlebyte(tmp.data, tmp.size);
		printf("optimal_code[%d] = %d\n", key_idx, key[key_idx]);
	}

	StringBuf decoded;
	sb_init(&decoded);
	for (int i = 0; i < s_len; i++) {
		int key_idx = i % keysize;
		sb_printf(&decoded, "%c", s[i] ^ key[key_idx]);
	}
	sb_hexdump(&decoded, "decoded");
}

int main(int argc, char **argv) {
// 	const char *s1 = "this is a test";
// 	const char *s2 = "wokka wokka!!!";
// 	printf("%s\n%s\ndistance: %d\n", s1, s2, hamming_dist(s1, strlen(s1), s2, strlen(s2)));
 	StringBuf s = sb_from_base64_file("6.txt");
	sb_hexdump(&s, "6.txt");
 	int keysize = guess_keysize(s.data, s.size);
 	printf("guessed keysize: %d\n", keysize);
	break_xor_for_keysize(s.data, s.size, 10);
}
