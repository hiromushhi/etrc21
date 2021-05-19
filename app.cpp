#include "app.h"

#include "device_io.h"

#if defined(MAKE_SIM)
static const bool kSimulator = true;
#else
static const bool kSimulator = false;
#endif

#if defined(MAKE_RIGHT)
static const bool kRcourse = true;
#else
static const bool kRcourse = false;
#endif

MotorIo* motor_io;
SensorIo* sensor_io;

static void initialize() {
  motor_io = new MotorIo();
  sensor_io = new SensorIo();
}

static void finalize() {
  delete sensor_io;
  delete motor_io;
}

void main_task(intptr_t unused) {
  initialize();
  sta_cyc(EXEC_ACTION_CYC);

  while (true) {
    if (ev3_button_is_pressed(BACK_BUTTON)) {
      break;
    }
    tslp_tsk(100*1000U);
  }

  stp_cyc(EXEC_ACTION_CYC);
  finalize();
  ext_tsk();
}

void exec_action_task(intptr_t unused) {
  ext_tsk();
}
