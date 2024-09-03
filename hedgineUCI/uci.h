#ifndef UCIFUNCTIONS_H
#define UCIFUNCTIONS_H

#include "hash.h"
#include "input.h"
#include "structs.h"
#include "engine.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include "sys/time.h"

extern gameInfo info;

void initializeAll();

long int getTimems();
  
int inputWaiting();

void readInput();



#endif
