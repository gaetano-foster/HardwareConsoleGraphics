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
    clock_t start, end; \
    start = clock(); \
    do { __VA_ARGS__ } while(0); \
    end = clock(); \
    VAR = ((double) (end - start) * 1000.0) / CLOCKS_PER_SEC; \
} while(0)


#endif