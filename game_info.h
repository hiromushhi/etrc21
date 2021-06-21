#ifndef ETRC21_GAME_INFO_H_
#define ETRC21_GAME_INFO_H_

const int kCircleNum = 28;
const int kNextToMax = 8;
const int kBlockNum = 9;

enum BlockId {
  kK1 = 0,
  kR1,
  kR2,
  kY1,
  kY2,
  kB1,
  kB2,
  kG1,
  kG2,
  kInvalidBlockId,
  kBlockIdNum,
};

struct Block;

struct Circle {
  char id;
  int x;
  int y;
  char color;
  Circle* next[kNextToMax];
  Block* block;
};

struct Block {
  BlockId id;
  char color;
  Circle* circle;
  Circle* target;
  bool carrying_completed;
};

class BingoArea {
 public:
  BingoArea(bool is_Rcourse);
  void SetBlockPos(BlockId block_id, char circle_id);
  Circle circles_[kCircleNum];
  Block blocks_[kBlockNum];

 private:
  void InitCircles();
  void InitBlocks();
  bool is_Rcourse_;
};

#endif  // ETRC21_GAME_INFO_H_
