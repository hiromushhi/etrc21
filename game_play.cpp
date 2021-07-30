#include "game_play.h"

#include <math.h>
#include <string.h>
#include <limits>

#include "ev3api.h"

const int kMax = std::numeric_limits<int>::max();
const int kRouteCharNum = 16;

ParamMaker::ParamMaker() {
}

DrivingParam ParamMaker::MakeForward(End end_type, Color end_color, float end_threshold) {
  DrivingParam p = { kGoForward, 8, 0, {}, end_type, end_color, end_threshold };
  return p;
}

DrivingParam ParamMaker::MakeBackward(End end_type, Color end_color, float end_threshold) {
  DrivingParam p = { kGoBackward, 8, 0, {}, end_type, end_color, end_threshold };
  return p;
}

DrivingParam ParamMaker::MakeLineTrace(End end_type, Color end_color, float end_threshold) {
  DrivingParam p = { kTraceRightEdge, 15, 50, { 0.4, 0, 0.05 }, end_type, end_color, end_threshold };
  return p;
}

DrivingParam ParamMaker::MakeRotateLeft(double theta) {
  DrivingParam p = { kRotateLeft, 8, 0, { }, kThetaEnd, kInvalidColor, static_cast<float>(theta*0.9) };
  return p;
}

DrivingParam ParamMaker::MakeRotateRight(double theta) {
  DrivingParam p = { kRotateRight, 8, 0, { }, kThetaEnd, kInvalidColor, static_cast<float>(theta*0.9) };
  return p;
}

ParamStore::ParamStore(BingoArea* bingo_area, RouteStore* route_store, ParamMaker* param_maker)
    : bingo_area_(bingo_area), route_store_(route_store), param_maker_(param_maker),
      is_wayback_(false), is_wayback_after_(false) {
}

double ParamStore::LimitRotationAngle(double angle) {
  double ret_angle = angle;
  if (fabs(angle) > M_PI) {
    if (angle >= 0)
      ret_angle = - (angle - M_PI);
    else
      ret_angle = - (angle + M_PI);
  }
  return ret_angle;
}

void ParamStore::AddTraceParam(Robot* robot, Circle* next_circle, Direction next_direction) {
  if (is_wayback_after_)
    is_wayback_after_ = false;
  else
    driving_params_.push_back(param_maker_->MakeForward(kDistanceEnd, kInvalidColor, 65));

  if (robot->direction != next_direction) {
    double from = static_cast<int>(robot->direction) * M_PI / 4;
    double to = static_cast<int>(next_direction) * M_PI / 4;
    double dtheta = LimitRotationAngle(to - from);
    if (dtheta > 0)
      driving_params_.push_back(param_maker_->MakeRotateLeft(dtheta));
    else
      driving_params_.push_back(param_maker_->MakeRotateRight(dtheta));
  }

  driving_params_.push_back(param_maker_->MakeForward(kDistanceEnd, kInvalidColor, 55));

  if (next_circle->color == 'R')
    driving_params_.push_back(param_maker_->MakeLineTrace(kColorEnd, kRed, 0));
  else if (next_circle->color == 'G')
    driving_params_.push_back(param_maker_->MakeLineTrace(kColorEnd, kGreen, 0));
  else if (next_circle->color == 'B')
    driving_params_.push_back(param_maker_->MakeLineTrace(kColorEnd, kBlue, 0));
  else if (next_circle->color == 'Y')
    driving_params_.push_back(param_maker_->MakeLineTrace(kColorEnd, kYellow, 0));

  robot->direction = next_direction;
  robot->circle = next_circle;
}

void ParamStore::AddPlaceParam(Robot* robot, Circle* next_circle, Direction next_direction) {
  if (is_wayback_) {
    driving_params_.push_back(param_maker_->MakeBackward(kDistanceEnd, kInvalidColor, -80));

    robot->circle = next_circle;
    is_wayback_ = false;
    is_wayback_after_ = true;
  } else {
    driving_params_.push_back(param_maker_->MakeForward(kDistanceEnd, kInvalidColor, 65));

    if (robot->direction != next_direction) {
      double from = static_cast<int>(robot->direction) * M_PI / 4;
      double to = static_cast<int>(next_direction) * M_PI / 4;
      double dtheta = LimitRotationAngle(to - from);
      if (dtheta > 0)
        driving_params_.push_back(param_maker_->MakeRotateLeft(dtheta));
      else
        driving_params_.push_back(param_maker_->MakeRotateRight(dtheta));
    }
    driving_params_.push_back(param_maker_->MakeForward(kDistanceEnd, kInvalidColor, 100));

    robot->direction = next_direction;
    robot->circle = next_circle;
    is_wayback_ = true;
  }
}

bool ParamStore::GenerateParam() {
  static bool is_entry = true;
  if (is_entry) {
    bingo_area_->InitRobot();
    is_entry = false;
  }

  std::list<char*>& routes = route_store_->routes_;
  if (routes.empty())
    return true;

  Robot* robot = &bingo_area_->robot_;
  char* route = routes.front();
  routes.pop_front();
  for (size_t i = 0; i < strlen(route); ++i) {
    Circle* next_circle = bingo_area_->SameIdCircle(route[i]);
    if (robot->circle->id == next_circle->id)
      continue;

    Direction next_direction = bingo_area_->DirectionToGo(robot->circle, next_circle);
    switch (next_direction) {
      case kEast:
      case kNorth:
      case kWest:
      case kSouth:
        AddTraceParam(robot, next_circle, next_direction);
        break;

      case kNorthEast:
      case kNorthWest:
      case kSouthWest:
      case kSouthEast:
        AddPlaceParam(robot, next_circle, next_direction);
        break;

      default:
        break;
    }
  }
  return false;
}

RouteStore::RouteStore(BingoArea* bingo_area, RouteSearch* route_search)
    : bingo_area_(bingo_area), route_search_(route_search) {
}

void RouteStore::SaveMovingRoute(Circle* goal_circle) {
  Circle* curr_circle = bingo_area_->robot_.circle;
  Circle* back_circle = NULL;

  char str[kRouteCharNum] = {};
  for (int i = 0; i < kRouteCharNum - 2; ++i) {
    str[i] = curr_circle->id;
    back_circle = curr_circle;
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

  if (goal_circle->id == '0')
    route_search_->reverse_circle_ = NULL;
  else
    route_search_->reverse_circle_ = back_circle;
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
}

RouteSearch::RouteSearch(BingoArea* bingo_area)
    : reverse_circle_(NULL), bingo_area_(bingo_area) {
}

void RouteSearch::ResetRouteSearchInfo() {
  for (int i = 0; i < kCircleNum; ++i) {
    Circle* circle = &bingo_area_->circles_[i];
    circle->prev = NULL;
    circle->queue_added = false;

    if ('1' <= circle->id && circle->id <= '9')
      circle->cost = kMax;
    else if (circle->block != NULL)
      circle->cost = kMax;
    else
      circle->cost = -1;
  }

  if (reverse_circle_ != NULL)
    reverse_circle_->cost = kMax;
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

BlockDecision::BlockDecision(BingoArea* bingo_area, BingoState* bingo_state)
    : bingo_area_(bingo_area), bingo_state_(bingo_state), carried_blocks_num_(0) {
}

Block* BlockDecision::NextCarryBlock() {
  Block* next_carry_block;

  if (carried_blocks_num_ == 0)
    next_carry_block = Select1stBlock();
  else if (carried_blocks_num_ == 1)
    next_carry_block = Select2ndBlock();
  else if (2 <= carried_blocks_num_ && carried_blocks_num_ <= 7)
    next_carry_block = Select3rdTo8thBlock();
  else if (carried_blocks_num_ == 8)
    next_carry_block = SelectBlackBlock();
  else
    next_carry_block = NULL;

  ++carried_blocks_num_;

  return next_carry_block;
}

Block* BlockDecision::Select1stBlock() {
  Robot* robot = &bingo_area_->robot_;
  Block* first_block = NULL;

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
      first_block = block;
      min = d;
    }
  }

  return first_block;
}

Block* BlockDecision::Select2ndBlock() {
  Block* second_block = NULL;

  double min = std::numeric_limits<double>::infinity();

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->carrying_completed)
      continue;

    double d = bingo_area_->DistanceFromCenter(block->circle);
    if (min > d) {
      second_block = block;
      min = d;
    }
  }

  return second_block;
}

Block* BlockDecision::Select3rdTo8thBlock() {
  Robot* robot = &bingo_area_->robot_;
  Block* third_to_eighth_block = NULL;

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->carrying_completed)
      continue;

    double d = bingo_area_->DistanceFromCenter(block->circle);
    if (3 > d) {
      third_to_eighth_block = block;
      return third_to_eighth_block;
    }
  }

  double min = std::numeric_limits<double>::infinity();
  bingo_state_->Update();

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->carrying_completed)
      continue;

    if (bingo_state_->IsGoodCandBlock(block)) {
      double d = bingo_area_->DistanceBtwCircles(robot->circle, block->circle)
               + bingo_area_->DistanceBtwCircles(block->circle, block->target);
      if (min > d) {
        third_to_eighth_block = block;
        min = d;
      }
    }
  }

  return third_to_eighth_block;
}

Block* BlockDecision::SelectBlackBlock() {
  Block* black_block = NULL;

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->color == 'K')
      black_block = block;
  }

  return black_block;
}

BingoAgent::BingoAgent(bool is_Rcourse)
    : calc_completed_(false), is_Rcourse_(is_Rcourse),
      curr_step_(kDecideCarryBlock), carry_block_(NULL) {
  bingo_area_ = new BingoArea(is_Rcourse_);
  bingo_state_ = new BingoState(bingo_area_);
  block_decision_ = new BlockDecision(bingo_area_, bingo_state_);
  route_search_ = new RouteSearch(bingo_area_);
  route_store_ = new RouteStore(bingo_area_, route_search_);
  param_maker_ = new ParamMaker();
  param_store_ = new ParamStore(bingo_area_, route_store_, param_maker_);
}

BingoAgent::~BingoAgent() {
  delete param_store_;
  delete param_maker_;
  delete route_store_;
  delete route_search_;
  delete block_decision_;
  delete bingo_state_;
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

    case kGenerateDrivingParam:
      GenerateDrivingParam();
      break;

    default:
      calc_completed_ = true;
      break;
  }
}

void BingoAgent::DecideCarryBlock() {
  carry_block_ = block_decision_->NextCarryBlock();

  if (carry_block_ == NULL)
    curr_step_ = kGenerateDrivingParam;
  else
    curr_step_ = kSearchMovingRoute;
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

void BingoAgent::GenerateDrivingParam() {
  bool generate_completed = param_store_->GenerateParam();

  if (generate_completed)
    curr_step_ = kBingoCompleted;
}
