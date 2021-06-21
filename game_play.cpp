#include "game_play.h"

BingoAgent::BingoAgent(bool is_Rcourse) : is_Rcourse_(is_Rcourse) {
  bingo_area_ = new BingoArea(is_Rcourse_);
}

BingoAgent::~BingoAgent() {
  delete bingo_area_;
}

void BingoAgent::SetBlockPos(BlockId block_id, char circle_id) {
  bingo_area_->SetBlockPos(block_id, circle_id);
}
