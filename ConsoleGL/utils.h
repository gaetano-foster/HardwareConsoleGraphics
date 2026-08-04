#ifndef __UTILS_H__
#define __UTILS_H__

#define EXPECT(B)								\
    do {										\
        if (!(B)) {								\
			fprintf(stderr,						\
                "%s:%d: EXPECT(%s) failed\n",	\
                __FILE__, __LINE__, #B);		\
            exit(EXIT_FAILURE);					\
		}										\
    } while (0)

#define TIME(VAR, ...) \
do { \
    LARGE_INTEGER start, end, freq; \
    QueryPerformanceFrequency(&freq); \
    QueryPerformanceCounter(&start); \
    do { __VA_ARGS__; } while (0); \
    QueryPerformanceCounter(&end); \
    (VAR) = ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)freq.QuadPart; \
} while (0)


#endif