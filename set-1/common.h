#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline
void
die1(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static inline
void
die(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, ": %m\n");
	exit(EXIT_FAILURE);
}

static inline
struct timespec timespec_now() {
	struct timespec t;
	if (clock_gettime(CLOCK_MONOTONIC, &t) != 0)
		die("clock_gettime");

	return t;
}

static inline
void
printok(bool ok, struct timespec *start, const char *fmt, ...)
{
	static int test_num = 0;
	va_list args;
	struct timespec now;
	int col = 0;

	if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
		die("clock_gettime");

	col += printf("%s %d\t- ", ok ? "ok" : "not ok", ++test_num);
	va_start(args, fmt);
	col += vprintf(fmt, args);
	va_end(args);

	int padding = col >= 48 ? 1 : 48 - col;
	printf("%*s", padding, "");
	int ms = (now.tv_sec - start->tv_sec) * 1000 + (now.tv_nsec - start->tv_nsec)/1000000;
	if (ms != 0) {
		printf("%d ms", ms);
	} else {
		int us = (now.tv_sec - start->tv_sec)*1000000 + (now.tv_nsec - start->tv_nsec)/1000;
		if (us != 0) {
			printf("%d us", us);
		} else {
			int ns = (now.tv_sec - start->tv_sec)*1000000000 + (now.tv_nsec - start->tv_nsec);
			printf("%d ns", ns);
		}

	}
	printf("\n");
}

static inline
int
max(int a, int b)
{
	if (a < b)
		return b;
	else
		return a;
}

#endif
