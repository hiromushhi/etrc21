#ifndef ETRC21_DRIVING_H_
#define ETRC21_DRIVING_H_

#include "device_io.h"
#include "etrc_info.h"
#include "utils.h"

class WheelsControl {
 public:
  WheelsControl(MotorIo* motor_io);
  void Exec(int8_t target_power_l, int8_t target_power_r);

 private:
  MotorIo* motor_io_;
};

class RlineTracer {
 public:
  RlineTracer(WheelsControl* wheels_control, Luminous* luminous);
  ~RlineTracer();
  void SetParam(TraceParam param);
  void Run();
  void Stop();

 private:
  WheelsControl* wheels_control_;
  Luminous* luminous_;
  Trace trace_type_;
  int8_t std_power_;
  float value_ref_;
  PidControl* pid_control_;
};

class EndCondition {
 public:
  EndCondition(Luminous* luminous, Localize* localize);
  void SetParam(EndParam param);
  bool IsSatisfied();

 private:
  Luminous* luminous_;
  Localize* localize_;
  Color end_color_;
  bool end_state_;
};

#endif  // ETRC21_DRIVING_H_
