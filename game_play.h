#ifndef ETRC21_GAME_PLAY_H_
#define ETRC21_GAME_PLAY_H_

#include "game_info.h"

class BingoAgent {
 public:
  BingoAgent(bool is_Rcourse);
  ~BingoAgent();
  void SetBlockPos(BlockId block_id, char circle_id);
  void SolveBingo();

 private:
  bool is_Rcourse_;
  BingoArea* bingo_area_;
};

#endif  // ETRC21_GAME_PLAY_H_
