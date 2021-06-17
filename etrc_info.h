#ifndef ETRC21_ETRC_INFO_H_
#define ETRC21_ETRC_INFO_H_

#include "ev3api.h"
#include "info_type.h"
#include "device_io.h"

class Luminous {
 public:
  Luminous(SensorIo* sensor_io);
  void Update();
  Color color_;
  Hsv hsv_;

 private:
  void SetColorReference(Color c, Hsv hsv);
  void UpdateHsv();
  void UpdateColor();
  SensorIo* sensor_io_;
  Hsv color_ref_[kColorNum];
};

class Localize {
 public:
  Localize(MotorIo* motor_io);
  void Update();
  Pose pose_;
  double distance_;

 private:
  MotorIo* motor_io_;
  float radius_;  // タイヤの半径 [mm]
  float tread_;  // 左右の車輪中心間距離 [mm]
};

#endif  // ETRC21_ETRC_INFO_H_
