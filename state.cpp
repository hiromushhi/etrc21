#include "state.h"

StateManager::StateManager(DrivingManager* driving_manager)
    : driving_manager_(driving_manager), state_(kTimeAttack) {
}

void StateManager::Update() {
  switch (state_) {
    case kTimeAttack:
      break;

    case kBlockBingo:
      break;

    default:
      break;
  }
}
