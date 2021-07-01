#include "game_play.h"

#include <string.h>
#include <limits>

#include "ev3api.h"

const int kMax = std::numeric_limits<int>::max();
const int kRouteCharNum = 16;

RouteStore::RouteStore(BingoArea* bingo_area) : bingo_area_(bingo_area) {
}

void RouteStore::SaveMovingRoute(Circle* goal_circle) {
  Circle* curr_circle = bingo_area_->robot_.circle;

  char str[kRouteCharNum] = {};
  for (int i = 0; i < kRouteCharNum - 2; ++i) {
    str[i] = curr_circle->id;
    curr_circle = curr_circle->prev;
    if (curr_circle->id == goal_circle->id) {
      str[i + 1] = curr_circle->id;
      str[i + 2] = '\0';
      break;
    }
  }
  char* route = new char(strlen(str) + 1);
  strcpy(route, str);
  routes_.push_back(route);
  syslog(LOG_NOTICE, str);
}

void RouteStore::SaveCarryRoute(Circle* goal_circle) {
  Circle* curr_circle = bingo_area_->robot_.circle;
  Circle* back_circle = NULL;

  char str[kRouteCharNum] = {};
  for (int i = 0; i < kRouteCharNum - 3; ++i) {
    str[i] = curr_circle->id;
    back_circle = curr_circle;
    curr_circle = curr_circle->prev;
    if (curr_circle->id == goal_circle->id) {
      str[i + 1] = curr_circle->id;
      str[i + 2] = back_circle->id;
      str[i + 3] = '\0';
      break;
    }
  }
  char* route = new char(strlen(str) + 1);
  strcpy(route, str);
  routes_.push_back(route);
  syslog(LOG_NOTICE, str);
}

RouteSearch::RouteSearch(BingoArea* bingo_area) : bingo_area_(bingo_area) {
}

void RouteSearch::ResetRouteSearchInfo() {
  for (int i = 0; i < kCircleNum; ++i) {
    Circle* circle = &bingo_area_->circles_[i];
    circle->prev = NULL;
    circle->queue_added = false;

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
  static bool is_entry = true;
  if (is_entry) {
    ResetRouteSearchInfo();
    queue_.push_back(goal_circle);
    is_entry = false;
  }

  Robot* robot = &bingo_area_->robot_;
  if (!queue_.empty()) {
    Circle* curr_circle = queue_.front();
    queue_.pop_front();
    if (curr_circle->id == robot->circle->id) {
      queue_.clear();
      is_entry = true;
      return true;
    }
    for (int i = 0; i < curr_circle->next_num; ++i) {
      Circle* next_circle = curr_circle->next[i];
      if (next_circle->queue_added)
        continue;

      if (next_circle->cost == -1 || next_circle->id == robot->circle->id) {
        next_circle->queue_added = true;
        next_circle->prev = curr_circle;
        queue_.push_back(next_circle);
      }
    }
  }
  return false;
}

void RouteSearch::MoveRobot(Circle* goal_circle, bool stepback) {
  Robot* robot = &bingo_area_->robot_;
  if (stepback) {
    Circle* curr_circle = robot->circle;
    Circle* back_circle = NULL;
    for (int i = 0; i < kCircleNum; ++i) {
      back_circle = curr_circle;
      curr_circle = curr_circle->prev;
      if (curr_circle->id == goal_circle->id) {
        break;
      }
    }
    robot->circle = back_circle;
  } else {
    robot->circle = goal_circle;
  }
}

void RouteSearch::CompleteCarryBlock(Block* block) {
  Circle* before_circle = block->circle;
  Circle* after_circle = block->target;

  before_circle->block = NULL;
  after_circle->block = block;

  block->circle = block->target;
  block->carrying_completed = true;
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
    : is_Rcourse_(is_Rcourse), curr_step_(kDecideCarryBlock), carry_block_(NULL) {
  bingo_area_ = new BingoArea(is_Rcourse_);
  block_decision_ = new BlockDecision(bingo_area_);
  route_search_ = new RouteSearch(bingo_area_);
  route_store_ = new RouteStore(bingo_area_);
}

BingoAgent::~BingoAgent() {
  delete route_store_;
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
    case kDecideCarryBlock:
      DecideCarryBlock();
      break;

    case kSearchMovingRoute:
      SearchMovingRoute();
      break;

    case kSearchCarryRoute:
      SearchCarryRoute();
      break;

    default:
      break;
  }
}

void BingoAgent::DecideCarryBlock() {
  carry_block_ = block_decision_->NextCarryBlock();

  if (carry_block_ == NULL) {
    curr_step_ = kBingoCompleted;
  } else {
    curr_step_ = kSearchMovingRoute;
  }
}

void BingoAgent::SearchMovingRoute() {
  bool is_found = route_search_->CalcMovingRoute(carry_block_->circle);

  if (is_found) {
    route_store_->SaveMovingRoute(carry_block_->circle);
    route_search_->MoveRobot(carry_block_->circle, false);
    curr_step_ = kSearchCarryRoute;
  }
}

void BingoAgent::SearchCarryRoute() {
  bool is_found = route_search_->CalcMovingRoute(carry_block_->target);

  if (is_found) {
    route_store_->SaveCarryRoute(carry_block_->target);
    route_search_->MoveRobot(carry_block_->target, true);
    route_search_->CompleteCarryBlock(carry_block_);
    curr_step_ = kDecideCarryBlock;
  }
}
