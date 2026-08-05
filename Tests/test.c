#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "../ConsoleGL/utils.h"

unsigned int
itos(unsigned int i, char *buf)
{
	int n = 0;
	unsigned int temp = i;

	do {
		n++;
		temp /= 10;
	} while (temp > 0);

	int idx = n - 1;
	do {
		buf[idx--] = (i % 10) + '0';
		i /= 10;
	} while (i > 0);

	return n;
}

static inline int
append_color(char *p, 
	unsigned int r, 
	unsigned int g, 
	unsigned int b)
{
	char *t = p; // save original pointer spot

	*p++ = '\x1b';
	*p++ = '[';
	*p++ = '3';
	*p++ = '8';
	*p++ = ';';
	*p++ = '2';
	*p++ = ';';
	p += itos(r, p);
	*p++ = ';';
	p += itos(g, p);
	*p++ = ';';
	p += itos(b, p);
	*p++ = 'm';
	*p++ = '█';

	return p - t;
}

void
main(void)
{
	double time_append = 0, time_sprintf = 0;
	char msg[128];
	char *p = msg;

	TIME(time_append,
		p += append_color(p, 255, 0, 255);
		*p++ = '\0';
	);

	p = msg;
	TIME(time_sprintf,
		p += sprintf(
			p,
			"\x1b[38;2;%d;%d;%dm█",
			255, 255, 255);
	);

	printf("MY TIME: %.6f\nSPRINTF TIME: %.6f\n", time_append, time_sprintf);
}