#ifndef ETRC21_DRIVING_H_
#define ETRC21_DRIVING_H_

#include "device_io.h"
#include "etrc_info.h"
#include "utils.h"

#include <list>

class WheelsControl {
 public:
  WheelsControl(MotorIo* motor_io);
  void Exec(int8_t target_power_l, int8_t target_power_r);

 private:
  MotorIo* motor_io_;
};

class LineTracer {
 public:
  LineTracer(WheelsControl* wheels_control, Luminous* luminous);
  ~LineTracer();
  void SetParam(Trace trace_type, int8_t ref_power, float ref_value, Gain gain);
  void Run();
  void Stop();

 private:
  WheelsControl* wheels_control_;
  Luminous* luminous_;
  Trace trace_type_;
  int8_t ref_power_;
  float ref_value_;
  PidControl* pid_control_;
};

class VlineTracer {
 public:
  VlineTracer(WheelsControl* wheels_control);
  ~VlineTracer();
  void SetParam(Trace trace_type, int8_t ref_power);
  void Run();
  void Stop();

 private:
  WheelsControl* wheels_control_;
  Localize* localize_;
  Trace trace_type_;
  int8_t ref_power_;
};

class EndCondition {
 public:
  EndCondition(Luminous* luminous, Localize* localize);
  void SetParam(End end_type, Color end_color, float end_threshold);
  bool IsSatisfied();

 private:
  Luminous* luminous_;
  Localize* localize_;
  End end_type_;
  Color end_color_;
  float end_threshold_;
  bool end_state_;
  float ref_distance_;
  float ref_theta_;
};

class DrivingManager {
 public:
  DrivingManager(LineTracer* line_tracer, VlineTracer* vline_tracer, EndCondition* end_condition);
  void Update();
  void AddDrivingParam(DrivingParam param);

 private:
  void SetTracerParam(DrivingParam& param);
  void SetEndParam(DrivingParam& param);
  void DriveTracer(DrivingParam& param);
  LineTracer* line_tracer_;
  VlineTracer* vline_tracer_;
  EndCondition* end_condition_;
  std::list<DrivingParam> driving_params_;
};

#endif  // ETRC21_DRIVING_H_
