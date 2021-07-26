#include "state.h"

const int kLcourseParamsNum = 5;
const DrivingParam kLcourseParams[kLcourseParamsNum] = {
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 6400 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 800 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceRightEdge, 30, 50, { 0.4, 0, 0.05 }, kColorEnd, kYellow, 0 },
};

const int kRcourseParamsNum = 5;
const DrivingParam kRcourseParams[kRcourseParamsNum] = {
  { kTraceLeftEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 6400 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceLeftEdge, 30, 50, { 0.4, 0, 0.05 }, kDistanceEnd, kInvalidColor, 800 },
  { kGoForward, 10, 0, { }, kDistanceEnd, kInvalidColor, 300 },
  { kTraceLeftEdge, 30, 50, { 0.4, 0, 0.05 }, kColorEnd, kYellow, 0 },
};

StateManager::StateManager(DrivingManager* driving_manager, BingoAgent* bingo_agent, bool is_Rcourse)
    : driving_manager_(driving_manager), bingo_agent_(bingo_agent), is_Rcourse_(is_Rcourse),
      state_(kTimeAttack) {
  if (is_Rcourse_) {
    for (int i = 0; i < kRcourseParamsNum; ++i) {
      driving_manager_->AddDrivingParam(kRcourseParams[i]);
    }
  } else {
    for (int i = 0; i < kLcourseParamsNum; ++i) {
      driving_manager_->AddDrivingParam(kLcourseParams[i]);
    }
  }
}

void StateManager::Update() {
  switch (state_) {
    case kTimeAttack:
      ActTimeAttack();
      break;

    case kBlockBingo:
      ActBlockBingo();
      break;

    default:
      break;
  }
}

void StateManager::ActTimeAttack() {
  driving_manager_->Update();
  if (driving_manager_->DrivingParamsEmpty()) {
    state_ = kBlockBingo;
  }
}

void StateManager::ActBlockBingo() {
  if (bingo_agent_->calc_completed_) {
    ParamStore* param_store = bingo_agent_->param_store_;
    std::list<DrivingParam>& driving_params = param_store->driving_params_;
    if (!driving_params.empty()) {
      DrivingParam driving_param = driving_params.front();
      driving_manager_->AddDrivingParam(driving_param);
      driving_params.pop_front();
    }
  }
  driving_manager_->Update();
}
