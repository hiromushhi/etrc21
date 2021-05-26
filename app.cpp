#include "app.h"

#include "device_io.h"
#include "etrc_info.h"
#include "driving.h"

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
Luminous* luminous;
Localize* localize;
SpeedMeter* speed_meter;
WheelsControl* wheels_control;
RlineTracer* rline_tracer;
VlineTracer* vline_tracer;
EndCondition* end_condition;
DrivingManager* driving_manager;

static void initialize() {
  motor_io = new MotorIo();
  sensor_io = new SensorIo();
  luminous = new Luminous(sensor_io);
  localize = new Localize(motor_io);
  speed_meter = new SpeedMeter(localize);
  wheels_control = new WheelsControl(motor_io);
  rline_tracer = new RlineTracer(wheels_control, luminous);
  vline_tracer = new VlineTracer(wheels_control);
  end_condition = new EndCondition(luminous, localize);
  driving_manager = new DrivingManager(rline_tracer, vline_tracer, end_condition);
}

static void finalize() {
  delete driving_manager;
  delete end_condition;
  delete vline_tracer;
  delete rline_tracer;
  delete wheels_control;
  delete speed_meter;
  delete localize;
  delete luminous;
  delete sensor_io;
  delete motor_io;
}

void main_task(intptr_t unused) {
  initialize();

  sta_cyc(UPDATE_INFO_CYC);

  while (true) {
    if (sensor_io->touch_sensor_pressed_) {
      break;
    }
    tslp_tsk(10*1000U);
  }

  sta_cyc(EXEC_ACTION_CYC);

  while (true) {
    if (sensor_io->back_button_pressed_) {
      break;
    }
    tslp_tsk(100*1000U);
  }

  stp_cyc(EXEC_ACTION_CYC);
  stp_cyc(UPDATE_INFO_CYC);
  finalize();
  ext_tsk();
}

void exec_action_task(intptr_t unused) {
  ext_tsk();
}

void update_info_task(intptr_t unused) {
  motor_io->Update();
  sensor_io->Update();
  luminous->Update();
  localize->Update();
  speed_meter->Update();
  ext_tsk();
}
