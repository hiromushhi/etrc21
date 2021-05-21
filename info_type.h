#ifndef ETRC21_INFO_TYPE_H_
#define ETRC21_INFO_TYPE_H_

struct Hsv {
  float h;
  float s;
  float v;
};

enum Color {
  kGreen = 0,
  kBlack,
  kRed,
  kYellow,
  kBlue,
  kWhite,
  kNone,
  kColorNum
};

struct Pose {
  float x;
  float y;
  float theta;
};

struct RlineTraceParam {
  bool left_edge;
  int8_t std_power;
  float value_ref;
  float kp;
  float ki;
  float kd;
};

#endif  // ETRC21_INFO_TYPE_H_
