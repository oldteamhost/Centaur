/*
 * CENTAUR
 * by oldteam & lomaster
 * license GPL-3.0
 *   Сделано от души 2023.
*/

#ifndef LOG_H 
#define LOG_H 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void sudorun(void);
void running(const char* version);
void version(const char* version);
void usage(const char *run);
void qutting(void);

#ifdef __cplusplus
}
#endif

#endif

