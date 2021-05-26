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
      end_type_(kIvalidEnd), end_color_(kInvalidColor), end_state_(false) {
}

void EndCondition::SetParam(End end_type, Color end_color) {
  end_type_ = end_type;
  end_color_ = end_color;
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
  if (driving_params_.empty()) {
    vline_tracer_->Stop();
    return;
  }

  curr_param_ = driving_params_.front();
  if (!curr_param_.is_started) {
    SetTracerParam();
    SetEndParam();
    curr_param_.is_started = true;
  }

  DriveTracer();
  if (EndConditionSatisfied()) {
    curr_param_.is_finished = true;
  }

  if (curr_param_.is_finished) {
    driving_params_.pop_front();
  }
}

void DrivingManager::AddDrivingParam(DrivingParam param) {
  driving_params_.push_back(param);
}

void DrivingManager::SetTracerParam() {
  Trace trace_type = curr_param_.trace_type;
  int8_t std_power = curr_param_.std_power;
  float value_ref = curr_param_.value_ref;
  float kp = curr_param_.kp;
  float ki = curr_param_.ki;
  float kd = curr_param_.kd;

  switch (trace_type) {
    case kRlineLeft:
    case kRlineRight:
      rline_tracer_->SetParam(trace_type, std_power, value_ref, kp, ki, kd);
      break;

    case kVlineForward:
    case kVlineBackward:
    case kVlineLeftRotation:
    case kVlineRightRotation:
      vline_tracer_->SetParam(trace_type, std_power);
      break;

    default:
      break;
  }
}

void DrivingManager::SetEndParam() {
  End end_type = curr_param_.end_type;
  Color end_color = curr_param_.end_color;

  end_condition_->SetParam(end_type, end_color);
}

void DrivingManager::DriveTracer() {
  Trace trace_type = curr_param_.trace_type;

  switch (trace_type) {
    case kRlineLeft:
    case kRlineRight:
      rline_tracer_->Run();
      break;

    case kVlineForward:
    case kVlineBackward:
    case kVlineLeftRotation:
    case kVlineRightRotation:
      vline_tracer_->Run();
      break;

    default:
      break;
  }
}

bool DrivingManager::EndConditionSatisfied() {
  return end_condition_->IsSatisfied();
}
