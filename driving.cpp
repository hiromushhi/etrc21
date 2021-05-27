#include "driving.h"

#include <math.h>

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
      trace_type_(kRlineLeft), ref_power_(0), ref_value_(0) {
  pid_control_ = new PidControl();
}

RlineTracer::~RlineTracer() {
  delete pid_control_;
}

void RlineTracer::SetParam(Trace trace_type, int8_t ref_power, float ref_value, float kp, float ki, float kd) {
  trace_type_ = trace_type;
  ref_power_ = ref_power;
  ref_value_ = ref_value;
  pid_control_->SetGain(kp, ki, kd);
}

void RlineTracer::Run() {
  float mv = pid_control_->GetMv(ref_value_, luminous_->hsv_.v);

  if (trace_type_ == kRlineLeft) {
    mv *= -1;
  }
  int8_t power_l = static_cast<int8_t>(ref_power_ + mv);
  int8_t power_r = static_cast<int8_t>(ref_power_ - mv);

  wheels_control_->Exec(power_l, power_r);
}

void RlineTracer::Stop() {
  wheels_control_->Exec(0, 0);
}

VlineTracer::VlineTracer(WheelsControl* wheels_control)
    : wheels_control_(wheels_control),
      trace_type_(kInvalidTrace), ref_power_(0) {
}

VlineTracer::~VlineTracer() {
}

void VlineTracer::SetParam(Trace trace_type, int8_t ref_power) {
  trace_type_ = trace_type;
  ref_power_ = ref_power;
}

void VlineTracer::Run() {
  int8_t power_l;
  int8_t power_r;

  if (trace_type_ == kVlineForward) {
    power_l = power_r = ref_power_;
  } else if (trace_type_ == kVlineBackward) {
    power_l = power_r = -ref_power_;
  } else if (trace_type_ == kVlineLeftRotation) {
    power_l = -ref_power_;
    power_r = ref_power_;
  } else if (trace_type_ == kVlineRightRotation) {
    power_l = ref_power_;
    power_r = -ref_power_;
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
      end_type_(kIvalidEnd), end_color_(kInvalidColor), end_distance_(0),
      end_state_(false), ref_distance_(0), ref_theta_(0) {
}

void EndCondition::SetParam(End end_type, Color end_color, float end_distance, float end_theta) {
  end_type_ = end_type;
  end_color_ = end_color;
  end_distance_ = end_distance;
  end_theta_ = end_theta;
  end_state_ = false;

  if (end_type_ == kDistanceEnd) {
    ref_distance_ = localize_->distance_;
  } else if (end_type_ == kThetaEnd) {
    ref_theta_ = localize_->pose_.theta;
  }
}

bool EndCondition::IsSatisfied() {
  switch (end_type_) {
    case kColorEnd:
      if (end_color_ == luminous_->color_)
        end_state_ = true;
      break;

    case kDistanceEnd:
      if (localize_->distance_ - ref_distance_ > end_distance_)
        end_state_ = true;
      break;

    case kThetaEnd:
      if (localize_->pose_.theta - ref_theta_ > end_theta_)
        end_state_ = true;
      break;

    default:
      break;
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

  DrivingParam& curr_param = driving_params_.front();
  if (!curr_param.is_started) {
    SetTracerParam(curr_param);
    SetEndParam(curr_param);
    curr_param.is_started = true;
  }

  DriveTracer(curr_param);
  if (EndConditionSatisfied()) {
    curr_param.is_finished = true;
  }

  if (curr_param.is_finished) {
    driving_params_.pop_front();
  }
}

void DrivingManager::AddDrivingParam(DrivingParam param) {
  driving_params_.push_back(param);
}

void DrivingManager::SetTracerParam(DrivingParam& param) {
  Trace trace_type = param.trace_type;
  int8_t ref_power = param.ref_power;
  float ref_value = param.ref_value;
  float kp = param.kp;
  float ki = param.ki;
  float kd = param.kd;

  switch (trace_type) {
    case kRlineLeft:
    case kRlineRight:
      rline_tracer_->SetParam(trace_type, ref_power, ref_value, kp, ki, kd);
      break;

    case kVlineForward:
    case kVlineBackward:
    case kVlineLeftRotation:
    case kVlineRightRotation:
      vline_tracer_->SetParam(trace_type, ref_power);
      break;

    default:
      break;
  }
}

void DrivingManager::SetEndParam(DrivingParam& param) {
  End end_type = param.end_type;
  Color end_color = param.end_color;
  float end_distance = param.end_distance;
  float end_theta = param.end_theta;

  end_condition_->SetParam(end_type, end_color, end_distance, end_theta);
}

void DrivingManager::DriveTracer(DrivingParam& param) {
  Trace trace_type = param.trace_type;

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
