#ifndef ETRC21_INFO_TYPE_H_
#define ETRC21_INFO_TYPE_H_

#include <stdint.h>

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

enum Move {
  kTraceLeftEdge = 0,
  kTraceRightEdge,
  kGoForward,
  kGoBackward,
  kRotateLeft,
  kRotateRight,
  kInvalidMove,
  kMoveNum
};

enum End {
  kColorEnd = 0,
  kDistanceEnd,
  kThetaEnd,
  kIvalidEnd,
  kEndNum
};

enum State {
  kTimeAttack = 0,
  kBlockBingo,
  kStateNum
};

struct Hsv {
  float h;
  float s;
  float v;
};

struct Pose {
  double x;
  double y;
  double theta;
};

struct Gain {
  float kp;
  float ki;
  float kd;
};

struct DrivingParam {
  Move move_type;
  int8_t ref_power;
  float ref_value;
  Gain gain;
  End end_type;
  Color end_color;
  float end_threshold;
  bool is_started;
  bool is_finished;
};

enum BlockId {
  kK1 = 0,
  kR1,
  kR2,
  kY1,
  kY2,
  kB1,
  kB2,
  kG1,
  kG2,
  kInvalidBlockId,
  kBlockIdNum,
};

#endif  // ETRC21_INFO_TYPE_H_
