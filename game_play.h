#ifndef ETRC21_GAME_PLAY_H_
#define ETRC21_GAME_PLAY_H_

#include "game_info.h"

class RouteSearch {
 public:
  RouteSearch(BingoArea* bingo_area);
  void ResetRouteSearchInfo();
  bool CalcMovingRoute(Circle* goal_circle);

 private:
  BingoArea* bingo_area_;
};

class BlockDecision {
 public:
  BlockDecision(BingoArea* bingo_area);
  Block* NextCarryBlock();

 private:
  BingoArea* bingo_area_;
};

enum Step {
  kCarryBlockUndecided = 0,
  kMovingRouteUnresolved,
  kCarryRouteUnresolved,
  kBingoCompleted,
};

class BingoAgent {
 public:
  BingoAgent(bool is_Rcourse);
  ~BingoAgent();
  void SetBlockPos(BlockId block_id, char circle_id);
  void UpdateBlockTarget();
  void TakeOneStep();

 private:
  void DecideNextCarryBlock();
  void SearchMovingRoute();
  bool is_Rcourse_;
  Step curr_step_;
  Block* next_carry_block_;
  BingoArea* bingo_area_;
  BlockDecision* block_decision_;
  RouteSearch* route_search_;
};

#endif  // ETRC21_GAME_PLAY_H_
