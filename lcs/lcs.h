#ifndef LCS_H
#define LCS_H
typedef int (Cmp)(const void *a, const void *b);

void *lcs(const void *a, int a_len, const void *b, int b_len, int el_size, Cmp comparator, int *res_len);

char *lcs_str(const char *a, const char *b);

#endif
