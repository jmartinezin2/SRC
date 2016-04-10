
#ifndef _DBGMEM_H_
#define _DBGMEM_H_

#include <stdlib.h>
//#include <crtdbg.h>

extern int mallocleft;
extern int windowsleft;


static inline void *mymalloc(int size)
{
	mallocleft++;
	return malloc(size);
}

static inline void myfree(void *p)
{
	free(p);
	mallocleft--;
}

static void constructor()
{
	windowsleft++;
}

static void destructor()
{
	windowsleft--;
}

#ifdef _DEBUG_MEMORY_
#define malloc mymalloc
#define free myfree
#include <assert.h>
static inline void check_memory()
{
	assert(mallocleft < 3);
	assert(windowsleft == 0);
}
#else
static inline void check_memory(){}	//do nothing
#endif


#endif
