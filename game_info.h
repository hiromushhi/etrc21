#ifndef ETRC21_GAME_INFO_H_
#define ETRC21_GAME_INFO_H_

class BingoArea {
 public:
  BingoArea(bool is_Rcourse);

 private:
  void InitCircles();
  bool is_Rcourse_;
};

#endif  // ETRC21_GAME_INFO_H_
