#include "state.h"

StateManager::StateManager(DrivingManager* driving_manager, bool is_Rcourse)
    : driving_manager_(driving_manager), is_Rcourse_(is_Rcourse),
      state_(kTimeAttack) {
  if (is_Rcourse_) {
    ;
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
}

void StateManager::ActBlockBingo() {
}
