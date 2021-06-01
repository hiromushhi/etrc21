#ifndef ETRC21_STATE_H_
#define ETRC21_STATE_H_

#include "driving.h"
#include "info_type.h"

const int kLcourseParamsNum = 7;
const DrivingParam kLcourseParams[kLcourseParamsNum] = {
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 6400 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 800 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kColorEnd, kYellow, 0 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 50 },
  { kRotateRight, 5, 0, { }, kThetaEnd, kInvalidColor, -1.48 },
};

class StateManager {
 public:
  StateManager(DrivingManager* driving_manager, bool is_Rcourse);
  void Update();

 private:
  void ActTimeAttack();
  void ActBlockBingo();
  DrivingManager* driving_manager_;
  bool is_Rcourse_;
  State state_;
};

#endif  // ETRC21_STATE_H_
