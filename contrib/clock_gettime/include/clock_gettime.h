#ifndef __CLOCK_GETTIME_H__
#define __CLOCK_GETTIME_H__

#define CLOCK_MONOTONIC     1

int clock_gettime (int, struct timespec*);

#endif