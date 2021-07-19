#include "game_info.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ev3api.h"

static const char* kLcourseCircleData[kCircleNum] = {
  "A,-6,6,R",
  "B,-2,6,R",
  "C,2,6,B",
  "D,6,6,B",
  "E,-6,2,R",
  "F,-2,2,R",
  "G,2,2,B",
  "H,6,2,B",
  "J,-6,-2,Y",
  "K,-2,-2,Y",
  "L,2,-2,G",
  "M,6,-2,G",
  "P,-6,-6,Y",
  "Q,-2,-6,Y",
  "R,2,-6,G",
  "S,6,-6,G",
  "W,-9,-10,G",
  "X,-2,-10,Y",
  "1,-4,4,Y",
  "2,0,4,G",
  "3,4,4,R",
  "4,-4,0,B",
  "5,4,0,Y",
  "6,-4,-4,G",
  "7,0,-4,R",
  "8,4,-4,B",
  "9,0,0,N",
  "0,-9,-5,R",
};

static const char* kRcourseCircleData[] = {
  "",
};

static const char* kBlockData[kBlockNum] = {
  "0,K",
  "1,R",
  "2,R",
  "3,Y",
  "4,Y",
  "5,B",
  "6,B",
  "7,G",
  "8,G",
};

static const char* kLcourseRobotPos = "X,4";

static const char* kRcourseRobotPos = "";

static const char* kPatternStrData[kPatternNum] = {
  "123",
  "146",
  "358",
  "678",
};

BingoArea::BingoArea(bool is_Rcourse) : is_Rcourse_(is_Rcourse) {
  InitCircles();
  InitBlocks();
  InitRobot();
}

void BingoArea::SetBlockPos(BlockId block_id, char circle_id) {
  for (int i = 0; i < kBlockNum; ++i) {
    Block* b = &blocks_[i];
    if (b->id != block_id)
      continue;

    for (int j = 0; j < kCircleNum; ++j) {
      Circle* c = &circles_[j];
      if (c->id == circle_id) {
        b->circle = c;
        c->block = b;
      }
    }
  }
}

void BingoArea::UpdateBlockTarget() {
  Block* b1;
  Block* b2;
  while (true) {
    b1 = TargetUndecidedBlock();
    if (b1 == NULL)
      break;

    if (b1->color == 'K') {
      UpdateCenterMarkBlock(b1);
      continue;
    }
    b2 = SameColorBlock(b1);

    Circle* c1 = NULL;
    Circle* c2 = NULL;
    for (int i = 0; i < kCircleNum; ++i) {
      Circle* c = &circles_[i];
      if ('1' <= c->id && c->id <= '9' && c->color == b1->color) {
        if (c1 == NULL)
          c1 = c;
        else
          c2 = c;
      }
      if (c2 != NULL)
        break;
    }

    double d1 = DistanceBtwCircles(b1->circle, c1) + DistanceBtwCircles(b2->circle, c2);
    double d2 = DistanceBtwCircles(b1->circle, c2) + DistanceBtwCircles(b2->circle, c1);

    if (d1 < d2) {
      b1->target = c1;
      b2->target = c2;
    } else if (d1 > d2) {
      b1->target = c2;
      b2->target = c1;
    } else {
      b1->target = c1;
      b2->target = c2;
    }
  }
}

void BingoArea::InitCircles() {
  char id, color;
  int x, y;

  for (int i = 0; i < kCircleNum; ++i) {
    if (is_Rcourse_) {
      sscanf(kRcourseCircleData[i], "%c,%d,%d,%c", &id, &x, &y, &color);
    } else {
      sscanf(kLcourseCircleData[i], "%c,%d,%d,%c", &id, &x, &y, &color);
    }
    circles_[i] = { id, x, y, color };
  }

  for (int i = 0; i < kCircleNum; ++i) {
    int ni = 0;
    Circle* c1 = &circles_[i];
    for (int j = 0; j < kCircleNum; ++j) {
      Circle* c2 = &circles_[j];
      if (c1->id == c2->id)
        continue;

      if (('1' <= c1->id && c1->id <= '9') || ('1' <= c2->id && c2->id <= '9')) {
        double distance = sqrt(pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2));
        if (distance <= 3) {
          c1->next[ni] = c2;
          ++ni;
        }
      } else {
        if (c1->x == c2->x && (abs(c1->y - c2->y) <= 7)) {
          c1->next[ni] = c2;
          ++ni;
        } else if (c1->y == c2->y && (abs(c1->x - c2->x) <= 7)) {
          c1->next[ni] = c2;
          ++ni;
        }
      }
    }
    c1->next_num = ni;
  }
}

void BingoArea::InitBlocks() {
  int id;
  char color;

  for (int i = 0; i < kBlockNum; ++i) {
    sscanf(kBlockData[i], "%d,%c", &id, &color);
    blocks_[i] = { static_cast<BlockId>(id), color };
  }
}

void BingoArea::InitRobot() {
  char circle_id;
  int direction;

  if (is_Rcourse_) {
    sscanf(kRcourseRobotPos, "%c,%d", &circle_id, &direction);
  } else {
    sscanf(kLcourseRobotPos, "%c,%d", &circle_id, &direction);
  }

  for (int i = 0; i < kCircleNum; ++i) {
    Circle* c = &circles_[i];
    if (c->id == circle_id) {
      robot_ = { c, static_cast<Direction>(direction) };
      break;
    }
  }
}

Block* BingoArea::TargetUndecidedBlock() {
  for (int i = 0; i < kBlockNum; ++i) {
    Block* b = &blocks_[i];
    if (b->target == NULL) {
      return b;
    }
  }
  return NULL;
}

void BingoArea::UpdateCenterMarkBlock(Block* block) {
  for (int i = 0; i < kCircleNum; ++i) {
    Circle* c = &circles_[i];
    if (c->id == '9') {
      block->target = c;
      break;
    }
  }
}

Block* BingoArea::SameColorBlock(Block* block) {
  for (int i = 0; i < kBlockNum; ++i) {
    Block* b = &blocks_[i];
    if (b->id == block->id) {
      continue;
    } else if (b->color == block->color) {
      return b;
    }
  }
  return NULL;
}

double BingoArea::DistanceBtwCircles(Circle* c1, Circle* c2) {
  return sqrt(pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2));
}

double BingoArea::DistanceFromCenter(Circle* c) {
  return sqrt(pow(c->x, 2) + pow(c->y, 2));
}

Circle* BingoArea::SameIdCircle(char circle_id) {
  for (int i = 0; i < kCircleNum; ++i) {
    Circle* c = &circles_[i];
    if (c->id == circle_id) {
      return c;
    }
  }
  return NULL;
}

Direction BingoArea::DirectionToGo(Circle* from, Circle* to) {
  int dx = to->x - from->x;
  int dy = to->y - from->y;

  if (dx == 0 && dy != 0) {
    if (dy > 0)
      return kNorth;
    else
      return kSouth;
  } else if (dy == 0 && dx != 0) {
    if (dx > 0)
      return kEast;
    else
      return kWest;
  } else {
    if (dx > 0 && dy > 0)
      return kNorthEast;
    else if (dx > 0 && dy < 0)
      return kSouthEast;
    else if (dx < 0 && dy > 0)
      return kNorthWest;
    else if (dx < 0 && dy < 0)
      return kSouthWest;
  }
  return kInvalidDirection;
}

BingoState::BingoState(BingoArea* bingo_area)
    : bingo_area_(bingo_area), max_count_(0) {
  for (int i = 0; i < kPatternNum; ++i) {
    strncpy(patterns_[i].str, kPatternStrData[i], kPatternStrLen);
    patterns_[i].count = 0;
    patterns_[i].is_completed = false;
  }
}

void BingoState::Update() {
  max_count_ = 0;
  for (int i = 0; i < kPatternNum; ++i) {
    patterns_[i].count = 0;
    patterns_[i].is_completed = false;
  }

  for (int i = 0; i < kCircleNum; ++i) {
    Circle* circle = &bingo_area_->circles_[i];
    if ('1' <= circle->id && circle->id <= '9') {
      if (circle->block == NULL)
        continue;

      for (int j = 0; j < kPatternNum; ++j) {
        if (strchr(patterns_[j].str, static_cast<int>(circle->id)) != NULL)
          patterns_[j].count += 1;

        if (3 <= patterns_[j].count)
          patterns_[j].is_completed = true;
      }
    }
  }

  for (int i = 0; i < kPatternNum; ++i) {
    if (patterns_[i].is_completed)
      continue;

    if (max_count_ < patterns_[i].count)
      max_count_ = patterns_[i].count;
  }

}

bool BingoState::IsGoodCandBlock(Block* cand_block) {
  bool is_suitable = false;

  for (int i = 0; i < kPatternNum; ++i) {
    if (patterns_[i].is_completed)
      continue;

    if (patterns_[i].count == max_count_ &&
        strchr(patterns_[i].str, static_cast<int>(cand_block->target->id)) != NULL) {
      is_suitable = true;
    }
  }

  return is_suitable;
}
