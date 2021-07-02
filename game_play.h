#ifndef ETRC21_GAME_PLAY_H_
#define ETRC21_GAME_PLAY_H_

#include "game_info.h"

#include <list>

class ParamStore {
 public:
  ParamStore(BingoArea* bingo_area);
  bool GenerateParam();

 private:
  BingoArea* bingo_area_;
};

class RouteStore {
 public:
  RouteStore(BingoArea* bingo_area);
  void SaveMovingRoute(Circle* goal_circle);
  void SaveCarryRoute(Circle* goal_circle);
  std::list<char*> routes_;

 private:
  BingoArea* bingo_area_;
};

class RouteSearch {
 public:
  RouteSearch(BingoArea* bingo_area);
  void ResetRouteSearchInfo();
  bool CalcMovingRoute(Circle* goal_circle);
  void MoveRobot(Circle* goal_circle, bool stepback);
  void CompleteCarryBlock(Block* block);

 private:
  BingoArea* bingo_area_;
  std::list<Circle*> queue_;
};

class BlockDecision {
 public:
  BlockDecision(BingoArea* bingo_area);
  Block* NextCarryBlock();

 private:
  BingoArea* bingo_area_;
};

enum Step {
  kDecideCarryBlock = 0,
  kSearchMovingRoute,
  kSearchCarryRoute,
  kGenerateDrivingParam,
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
  void DecideCarryBlock();
  void SearchMovingRoute();
  void SearchCarryRoute();
  void GenerateDrivingParam();
  bool is_Rcourse_;
  Step curr_step_;
  Block* carry_block_;
  BingoArea* bingo_area_;
  BlockDecision* block_decision_;
  RouteSearch* route_search_;
  RouteStore* route_store_;
  ParamStore* param_store_;
};

#endif  // ETRC21_GAME_PLAY_H_
