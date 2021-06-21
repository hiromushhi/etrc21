#include "game_info.h"

#include <stdlib.h>
#include <math.h>

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

BingoArea::BingoArea(bool is_Rcourse) : is_Rcourse_(is_Rcourse) {
  InitCircles();
  InitBlocks();
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
