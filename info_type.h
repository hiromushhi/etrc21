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

#endif  // ETRC21_INFO_TYPE_H_
