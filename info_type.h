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

enum Trace {
  kRlineLeft = 0,
  kRlineRight,
  kVlineForward,
  kVlineBackward,
  kVlineLeftRotation,
  kVlineRightRotation,
  kTraceNum
};

struct TraceParam {
  Trace trace_type;
  int8_t std_power;
  float value_ref;
  float kp;
  float ki;
  float kd;
};

struct EndParam {
  Color color;
};

#endif  // ETRC21_INFO_TYPE_H_
