#ifndef ETRC21_STATE_H_
#define ETRC21_STATE_H_

#include "driving.h"
#include "game_play.h"

enum State {
  kTimeAttack = 0,
  kBlockBingo,
  kStateNum
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
