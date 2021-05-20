#ifndef ETRC21_APP_H_
#define ETRC21_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

#ifndef STACK_SIZE
#define STACK_SIZE 4096
#endif

#define EXEC_ACTION_DT_MS 10
#define UPDATE_INFO_DT_MS 10

#ifndef TOPPERS_MACRO_ONLY
extern void main_task(intptr_t exinf);
extern void exec_action_task(intptr_t exinf);
extern void update_info_task(intptr_t exinf);
#endif

#ifdef __cplusplus
}
#endif

#endif  // ETRC21_APP_H_
