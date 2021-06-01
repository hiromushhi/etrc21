#ifndef ETRC21_STATE_H_
#define ETRC21_STATE_H_

#include "driving.h"
#include "info_type.h"

class StateManager {
 public:
  StateManager(DrivingManager* driving_manager);
  void Update();

 private:
  DrivingManager* driving_manager_;
  State state_;
};

#endif  // ETRC21_STATE_H_
