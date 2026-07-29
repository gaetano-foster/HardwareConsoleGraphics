#ifndef __CONTEXT_H__
#define __CONTEXT_H__

BOOL
context_init(int width, int height);

void
context_destroy();

void
context_swap();

#endif