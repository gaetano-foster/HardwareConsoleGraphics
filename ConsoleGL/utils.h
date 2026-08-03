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

#define TIME(VAR, ACTION) \
do { \
    clock_t start, end; \
    double time_millis; \
    start = clock(); \
    do { ACTION } while(0); \
    end = clock(); \
    VAR = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000); \
} while(0)


#endif