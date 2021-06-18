#ifndef ETRC21_GAME_INFO_H_
#define ETRC21_GAME_INFO_H_

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

class BingoArea {
 public:
  BingoArea(bool is_Rcourse);

 private:
  void InitCircles();
  bool is_Rcourse_;
};

#endif  // ETRC21_GAME_INFO_H_
