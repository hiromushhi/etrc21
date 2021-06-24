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

enum Direction {
  kNorth = 0,
  kSouth,
  kEast,
  kWest,
  kNorthEast,
  kSouthEast,
  kNorthWest,
  kSouthWest,
  kInvalidDirection,
  kDirectionNum,
};

struct Block;

struct Circle {
  char id;
  int x;
  int y;
  char color;
  Circle* next[kNextToMax];
  Block* block;
  int cost;
  Circle* prev;
  bool is_fixed;
};

struct Block {
  BlockId id;
  char color;
  Circle* circle;
  Circle* target;
  bool carrying_completed;
};

struct Robot {
  Circle* circle;
  Direction direction;
};

class BingoArea {
 public:
  BingoArea(bool is_Rcourse);
  void SetBlockPos(BlockId block_id, char circle_id);
  void UpdateBlockTarget();
  double DistanceBtwCircles(Circle* c1, Circle* c2);
  Circle circles_[kCircleNum];
  Block blocks_[kBlockNum];
  Robot robot_;

 private:
  void InitCircles();
  void InitBlocks();
  void InitRobot();
  Block* TargetUndecidedBlock();
  void UpdateCenterMarkBlock(Block* block);
  Block* SameColorBlock(Block* block);
  bool is_Rcourse_;
};

#endif  // ETRC21_GAME_INFO_H_
