#ifndef ETRC21_STATE_H_
#define ETRC21_STATE_H_

#include "driving.h"
#include "game_play.h"
#include "info_type.h"

const int kLcourseParamsNum = 5;
const DrivingParam kLcourseParams[kLcourseParamsNum] = {
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 6400 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 800 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kColorEnd, kYellow, 0 },
};

class StateManager {
 public:
  StateManager(DrivingManager* driving_manager, BingoAgent* bingo_agent, bool is_Rcourse);
  void Update();

 private:
  void ActTimeAttack();
  void ActBlockBingo();
  DrivingManager* driving_manager_;
  BingoAgent* bingo_agent_;
  bool is_Rcourse_;
  State state_;
};

#endif  // ETRC21_STATE_H_
