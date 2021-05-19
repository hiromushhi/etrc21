#ifndef ETRC21_APP_H_
#define ETRC21_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

#ifndef STACK_SIZE
#define STACK_SIZE 4096
#endif

#ifndef TOPPERS_MACRO_ONLY
extern void main_task(intptr_t exinf);
#endif

#ifdef __cplusplus
}
#endif

#endif  // ETRC21_APP_H_
