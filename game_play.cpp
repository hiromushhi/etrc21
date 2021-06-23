#include "game_play.h"

#include <limits>

#include "ev3api.h"

BlockDecision::BlockDecision(BingoArea* bingo_area) : bingo_area_(bingo_area) {
}

Block* BlockDecision::NextCarryBlock() {
  Robot* robot = &bingo_area_->robot_;
  Block* next_block = NULL;

  double min = std::numeric_limits<double>::infinity();
  const double kDecisionCoeff = 1.5;

  for (int i = 0; i < kBlockNum; ++i) {
    Block* block = &bingo_area_->blocks_[i];
    if (block->carrying_completed)
      continue;

    double d = bingo_area_->DistanceBtwCircles(robot->circle, block->circle);
    if (robot->circle->x != block->circle->x &&
        robot->circle->y != block->circle->y) {
      d *= kDecisionCoeff;
    }

    if (min > d) {
      next_block = block;
      min = d;
    }
  }

  return next_block;
}

BingoAgent::BingoAgent(bool is_Rcourse) : is_Rcourse_(is_Rcourse) {
  bingo_area_ = new BingoArea(is_Rcourse_);
  block_decision_ = new BlockDecision(bingo_area_);
}

BingoAgent::~BingoAgent() {
  delete block_decision_;
  delete bingo_area_;
}

void BingoAgent::SetBlockPos(BlockId block_id, char circle_id) {
  bingo_area_->SetBlockPos(block_id, circle_id);
}

void BingoAgent::SolveBingo() {
  bingo_area_->UpdateBlockTarget();

  Block* b = block_decision_->NextCarryBlock();
}
