#ifndef ETRC21_GAME_PLAY_H_
#define ETRC21_GAME_PLAY_H_

#include "game_info.h"
#include "info_type.h"

#include <list>

class ParamMaker {
 public:
  ParamMaker();
  DrivingParam MakeForward(End end_type, Color end_color, float end_threshold);
  DrivingParam MakeBackward(End end_type, Color end_color, float end_threshold);
  DrivingParam MakeLineTrace(End end_type, Color end_color, float end_threshold);
  DrivingParam MakeRotateLeft(double theta);
  DrivingParam MakeRotateRight(double theta);
};

class RouteStore;

class ParamStore {
 public:
  ParamStore(BingoArea* bingo_area, RouteStore* route_store, ParamMaker* param_maker);
  bool GenerateParam();
  std::list<DrivingParam> driving_params_;

 private:
  double LimitRotationAngle(double angle);
  void AddTraceParam(Robot* robot, Circle* next_circle, Direction next_direction);
  void AddPlaceParam(Robot* robot, Circle* next_circle, Direction next_direction);
  BingoArea* bingo_area_;
  RouteStore* route_store_;
  ParamMaker* param_maker_;
  bool is_wayback_;
  bool is_wayback_after_;
};

class RouteSearch;

class RouteStore {
 public:
  RouteStore(BingoArea* bingo_area, RouteSearch* route_search);
  void SaveMovingRoute(Circle* goal_circle);
  void SaveCarryRoute(Circle* goal_circle);
  std::list<char*> routes_;

 private:
  BingoArea* bingo_area_;
  RouteSearch* route_search_;
};

class RouteSearch {
 public:
  RouteSearch(BingoArea* bingo_area);
  void ResetRouteSearchInfo();
  bool CalcMovingRoute(Circle* goal_circle);
  void MoveRobot(Circle* goal_circle, bool stepback);
  void CompleteCarryBlock(Block* block);
  Circle* reverse_circle_;

 private:
  BingoArea* bingo_area_;
  std::list<Circle*> queue_;
};

class BlockDecision {
 public:
  BlockDecision(BingoArea* bingo_area, BingoState* bingo_state);
  Block* NextCarryBlock();

 private:
  Block* Select1stBlock();
  Block* Select2ndBlock();
  Block* Select3rdTo8thBlock();
  Block* SelectBlackBlock();
  BingoArea* bingo_area_;
  BingoState* bingo_state_;
  int carried_blocks_num_;
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
  bool calc_completed_;
  ParamStore* param_store_;

 private:
  void DecideCarryBlock();
  void SearchMovingRoute();
  void SearchCarryRoute();
  void GenerateDrivingParam();
  bool is_Rcourse_;
  Step curr_step_;
  Block* carry_block_;
  BingoArea* bingo_area_;
  BingoState* bingo_state_;
  BlockDecision* block_decision_;
  RouteSearch* route_search_;
  ParamMaker* param_maker_;
  RouteStore* route_store_;
};

#endif  // ETRC21_GAME_PLAY_H_
