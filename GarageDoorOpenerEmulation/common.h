#ifndef __common_h__
#define __common_h__

#include <pthread.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>

//#define KBINTR 1	//interrupt vector number for QNX

extern pthread_mutex_t mut;
volatile extern char input;
volatile extern int inputGrabbed;

#endif

