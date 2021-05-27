#ifndef ETRC21_INFO_TYPE_H_
#define ETRC21_INFO_TYPE_H_

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
  kThetaEnd,
  kIvalidEnd,
  kEndNum
};

struct Hsv {
  float h;
  float s;
  float v;
};

struct Pose {
  float x;
  float y;
  float theta;
};

struct Gain {
  float kp;
  float ki;
  float kd;
};

struct DrivingParam {
  Trace trace_type;
  int8_t ref_power;
  float ref_value;
  Gain gain;
  End end_type;
  Color end_color;
  float end_distance;
  float end_theta;
  bool is_started;
  bool is_finished;
};

#endif  // ETRC21_INFO_TYPE_H_
