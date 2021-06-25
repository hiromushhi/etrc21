#include "game_play.h"

#include <limits>

#include "ev3api.h"

const int kMax = std::numeric_limits<int>::max();

RouteSearch::RouteSearch(BingoArea* bingo_area) : bingo_area_(bingo_area) {
}

void RouteSearch::ResetRouteSearchInfo() {
  for (int i = 0; i < kCircleNum; ++i) {
    Circle* circle = &bingo_area_->circles_[i];
    circle->prev = NULL;
    circle->is_fixed = false;

    if ('1' <= circle->id && circle->id <= '9') {
      circle->cost = kMax;
    } else if (circle->block != NULL) {
      circle->cost = kMax;
    } else {
      circle->cost = -1;
    }
  }
}

bool RouteSearch::CalcMovingRoute(Circle* goal_circle) {
  static Circle* curr_circle = NULL;

  if (!goal_circle->is_fixed) {
    goal_circle->cost = 0;
    goal_circle->is_fixed = true;
    curr_circle = goal_circle;
  }

  static int i = 0;
  Circle* next_circle = curr_circle->next[i];
  Robot* robot = &bingo_area_->robot_;
  static bool dead_end = true;

  if (next_circle != NULL) {
    if (next_circle->cost == -1 || next_circle->id == robot->circle->id) {
      next_circle->cost = 0;
      next_circle->prev = curr_circle;
      dead_end = false;
      if (next_circle->id == robot->circle->id) {
        return true;
      }
    }
  }

  ++i;
  if (i >= kNextToMax) {
    i = 0;
    curr_circle->is_fixed = true;
    if (dead_end) {
      curr_circle = curr_circle->prev;
    }

    for (int j = 0; j < kNextToMax; ++j) {
      if (!curr_circle->next[j]->is_fixed && curr_circle->next[j]->cost != kMax) {
        curr_circle = curr_circle->next[j];
        dead_end = true;
        break;
      }
    }
  }
  return false;
}

BlockDecision::BlockDecision(BingoArea* bingo_area) : bingo_area_(bingo_area) {
}

Block* BlockDecision::NextCarryBlock() {
  Robot* robot = &bingo_area_->robot_;
  Block* next_carry_block = NULL;

  double min = std::numeric_limits<double>::infinity();
  const double kDecisionCoeff = 1.5;

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->carrying_completed)
      continue;

    double d = bingo_area_->DistanceBtwCircles(robot->circle, block->circle);
    if (robot->circle->x != block->circle->x &&
        robot->circle->y != block->circle->y) {
      d *= kDecisionCoeff;
    }

    if (min > d) {
      next_carry_block = block;
      min = d;
    }
  }

  return next_carry_block;
}

BingoAgent::BingoAgent(bool is_Rcourse)
    : is_Rcourse_(is_Rcourse), curr_step_(kCarryBlockUndecided), next_carry_block_(NULL) {
  bingo_area_ = new BingoArea(is_Rcourse_);
  block_decision_ = new BlockDecision(bingo_area_);
  route_search_ = new RouteSearch(bingo_area_);
}

BingoAgent::~BingoAgent() {
  delete route_search_;
  delete block_decision_;
  delete bingo_area_;
}

void BingoAgent::SetBlockPos(BlockId block_id, char circle_id) {
  bingo_area_->SetBlockPos(block_id, circle_id);
}

void BingoAgent::UpdateBlockTarget() {
  bingo_area_->UpdateBlockTarget();
}

void BingoAgent::TakeOneStep() {
  switch (curr_step_) {
    case kCarryBlockUndecided:
      DecideNextCarryBlock();
      break;

    case kMovingRouteUnresolved:
      SearchMovingRoute();
      break;

    default:
      break;
  }
}

void BingoAgent::DecideNextCarryBlock() {
  next_carry_block_ = block_decision_->NextCarryBlock();

  if (next_carry_block_ == NULL) {
    curr_step_ = kBingoCompleted;
  } else {
    curr_step_ = kMovingRouteUnresolved;
  }
}

void BingoAgent::SearchMovingRoute() {
  static bool is_entry = true;
  if (is_entry) {
    route_search_->ResetRouteSearchInfo();
    is_entry = false;
  }

  if (route_search_->CalcMovingRoute(next_carry_block_->circle)) {
    curr_step_ = kCarryRouteUnresolved;
    is_entry = true;
    syslog(LOG_NOTICE, "transition kCarryRouteUnresolved");
  }
}
