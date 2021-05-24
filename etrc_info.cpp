#include "etrc_info.h"

#include <math.h>

#include "app.h"

Luminous::Luminous(SensorIo* sensor_io)
    : color_(kNone), hsv_({0, 0, 0}), sensor_io_(sensor_io) {
  SetColorReference(kGreen, (Hsv){120, 0, 0});
  SetColorReference(kBlack, (Hsv){0, 0, 0});
  SetColorReference(kRed, (Hsv){0, 0, 0});
  SetColorReference(kYellow, (Hsv){50, 0, 0});
  SetColorReference(kBlue, (Hsv){210, 0, 0});
  SetColorReference(kWhite, (Hsv){0, 20, 0});
  SetColorReference(kNone, (Hsv){0, 0, 0});
}

void Luminous::Update() {
  UpdateHsv();
  UpdateColor();
}

void Luminous::SetColorReference(Color c, Hsv hsv) {
  color_ref_[c] = hsv;
}

void Luminous::UpdateHsv() {
  rgb_raw_t val = sensor_io_->color_rgb_raw_;
  float r = static_cast<float>(val.r);
  float g = static_cast<float>(val.g);
  float b = static_cast<float>(val.b);

  float max = r > g ? r : g;
  max = max > b ? max : b;
  float min = r < g ? r : g;
  min = min < b ? min : b;
  float c = max - min;

  float h;
  if (c == 0) {
    h = -1;
  } else if (max == r) {
    h = fmodf(((g - b) / c), 6);
  } else if (max == g) {
    h = ((b - r) / c) + 2;
  } else if (max == b) {
    h = ((r - g) / c) + 4;
  } else {
    h = -1;
  }

  if (h != -1) {
    h = 60 * h;
  }

  float s;
  if (max == 0) {
    s = 0;
  } else {
    s = c / max;
  }

  float v = max;
  if (v > 100) {
    v = 100;
  }

  hsv_.h = h;
  hsv_.s = s * 100;
  hsv_.v = v;
}

void Luminous::UpdateColor() {
  float sat_white = color_ref_[kWhite].s;
  float val_black = color_ref_[kBlack].v;
  float hue_green = color_ref_[kGreen].h;
  float hue_red = color_ref_[kRed].h;
  float hue_blue = color_ref_[kBlue].h;
  float hue_yellow = color_ref_[kYellow].h;

  if (sat_white - 20 < hsv_.s && hsv_.s < sat_white + 20) {
    color_ = kWhite;
  } else {
    if (hue_green - 20 < hsv_.h && hsv_.h < hue_green + 70) {
      if (hsv_.v < val_black + 20) {
        color_ = kBlack;
      } else {
        color_ = kGreen;
      }
    } else if (hsv_.h < hue_red + 20 || 360 - 20 < hsv_.h) {
      color_ = kRed;
    } else if (hue_blue - 20 < hsv_.h && hsv_.h < hue_blue + 20) {
      color_ = kBlue;
    } else if (hue_yellow - 20 < hsv_.h && hsv_.h < hue_yellow + 20) {
      color_ = kYellow;
    } else {
      color_ = kNone;
    }
  }
}

Localize::Localize(MotorIo* motor_io)
    : pose_({0, 0, 0}), distance_(0), motor_io_(motor_io),
      radius_(45), tread_(140), dtheta_th_(0.001) {
}

void Localize::Update() {
  static int32_t prev_counts_l_ = 0;
  static int32_t prev_counts_r_ = 0;

  int32_t counts_l = motor_io_->counts_l_;
  int32_t counts_r = motor_io_->counts_r_;

  float dPhiL = (counts_l - prev_counts_l_) * M_PI / 180;
  float dPhiR = (counts_r - prev_counts_r_) * M_PI / 180;

  float dLL = radius_ * dPhiL;
  float dLR = radius_ * dPhiR;
  float dL = (dLR + dLL) / 2;
  float dtheta = (dLR - dLL) / tread_;

  pose_.theta = pose_.theta + dtheta;
  if (dtheta < dtheta_th_) {
    pose_.x = pose_.x + dL * cos(pose_.theta + dtheta / 2);
    pose_.y = pose_.y + dL * sin(pose_.theta + dtheta / 2);
  } else {
    float rho = dL / dtheta;
    float dLprime = 2 * rho * sin(dtheta / 2);
    pose_.x = pose_.x + dLprime * cos(pose_.theta + dtheta / 2);
    pose_.y = pose_.y + dLprime * sin(pose_.theta + dtheta / 2);
  }

  prev_counts_l_ = counts_l;
  prev_counts_r_ = counts_r;

  distance_ = ((counts_l + counts_r) / 2.0) * M_PI / 180 * radius_;
}

SpeedMeter::SpeedMeter(Localize* localize)
    : speed_(0), localize_(localize), dt_(UPDATE_INFO_DT_MS/1000.0) {
}

void SpeedMeter::Update() {
  static float prev_distance = 0;

  float distance = localize_->distance_;
  speed_ = (distance - prev_distance) / dt_;

  prev_distance = distance;
}
