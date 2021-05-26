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
  kInvalidColor,
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
  kInvalidTrace,
  kTraceNum
};

enum End {
  kColorEnd = 0,
  kDistanceEnd,
  kIvalidEnd,
  kEndNum
};

struct DrivingParam {
  Trace trace_type;
  int8_t std_power;
  float value_ref;
  float kp;
  float ki;
  float kd;
  End end_type;
  Color end_color;
  bool is_started;
  bool is_finished;
};

#endif  // ETRC21_INFO_TYPE_H_
