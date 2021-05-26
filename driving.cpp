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
      trace_type_(kRlineLeft), std_power_(0), value_ref_(0) {
  pid_control_ = new PidControl();
}

RlineTracer::~RlineTracer() {
  delete pid_control_;
}

void RlineTracer::SetParam(Trace trace_type, int8_t std_power, float value_ref, float kp, float ki, float kd) {
  trace_type_ = trace_type;
  std_power_ = std_power;
  value_ref_ = value_ref;
  pid_control_->SetGain(kp, ki, kd);
}

void RlineTracer::Run() {
  float mv = pid_control_->GetMv(value_ref_, luminous_->hsv_.v);

  if (trace_type_ == kRlineLeft) {
    mv *= -1;
  }
  int8_t power_l = static_cast<int8_t>(std_power_ + mv);
  int8_t power_r = static_cast<int8_t>(std_power_ - mv);

  wheels_control_->Exec(power_l, power_r);
}

void RlineTracer::Stop() {
  wheels_control_->Exec(0, 0);
}

VlineTracer::VlineTracer(WheelsControl* wheels_control)
    : wheels_control_(wheels_control),
      trace_type_(kInvalidTrace), std_power_(0) {
}

VlineTracer::~VlineTracer() {
}

void VlineTracer::SetParam(Trace trace_type, int8_t std_power) {
  trace_type_ = trace_type;
  std_power_ = std_power;
}

void VlineTracer::Run() {
  int8_t power_l;
  int8_t power_r;

  if (trace_type_ == kVlineForward) {
    power_l = power_r = std_power_;
  } else if (trace_type_ == kVlineBackward) {
    power_l = power_r = -std_power_;
  } else if (trace_type_ == kVlineLeftRotation) {
    power_l = -std_power_;
    power_r = std_power_;
  } else if (trace_type_ == kVlineRightRotation) {
    power_l = std_power_;
    power_r = -std_power_;
  } else {
    power_l = power_r = 0;
  }

  wheels_control_->Exec(power_l, power_r);
}

void VlineTracer::Stop() {
  wheels_control_->Exec(0, 0);
}

EndCondition::EndCondition(Luminous* luminous, Localize* localize)
    : luminous_(luminous), localize_(localize),
      end_color_(kNone), end_state_(false) {
}

void EndCondition::SetParam(EndParam param) {
  end_color_ = param.color;
  end_state_ = false;
}

bool EndCondition::IsSatisfied() {
  if (end_color_ == luminous_->color_) {
    end_state_ = true;
  }
  return end_state_;
}

DrivingManager::DrivingManager(RlineTracer* rline_tracer, VlineTracer* vline_tracer, EndCondition* end_condition)
    : rline_tracer_(rline_tracer), vline_tracer_(vline_tracer), end_condition_(end_condition) {
}

void DrivingManager::Update() {

}
