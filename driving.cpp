#include "driving.h"

WheelsControl::WheelsControl(MotorIo* motor_io) : motor_io_(motor_io) {
}

void WheelsControl::Exec(int8_t target_power_l, int8_t target_power_r) {
  int8_t curr_power_l = motor_io_->power_l_;
  int8_t curr_power_r = motor_io_->power_r_;

  if (target_power_l > curr_power_l) {
    curr_power_l += 1;
  } else if (target_power_l < curr_power_l) {
    curr_power_l -= 1;
  }

  if (target_power_r > curr_power_r) {
    curr_power_r += 1;
  } else if (target_power_r < curr_power_r) {
    curr_power_r -= 1;
  }

  motor_io_->SetWheelsPower(curr_power_l, curr_power_r);
}

RlineTracer::RlineTracer(WheelsControl* wheels_control, Luminous* luminous)
    : wheels_control_(wheels_control), luminous_(luminous),
      left_edge_(true), std_power_(0), value_ref_(0) {
  pid_control_ = new PidControl();
}

RlineTracer::~RlineTracer() {
  delete pid_control_;
}

void RlineTracer::SetParam(RlineTraceParam param) {
  left_edge_ = param.left_edge;
  std_power_ = param.std_power;
  value_ref_ = param.value_ref;
  pid_control_->SetGain(param.kp, param.ki, param.kd);
}

void RlineTracer::Run() {
  float mv = pid_control_->GetMv(value_ref_, luminous_->hsv_.v);

  if (left_edge_) {
    mv *= -1;
  }
  int8_t power_l = static_cast<int8_t>(std_power_ + mv);
  int8_t power_r = static_cast<int8_t>(std_power_ - mv);

  wheels_control_->Exec(power_l, power_r);
}
