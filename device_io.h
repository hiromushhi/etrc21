#ifndef ETRC21_DEVICE_IO_H_
#define ETRC21_DEVICE_IO_H_

#include "ev3api.h"

class MotorIo {
 public:
  MotorIo();
  ~MotorIo();
  void Update();
  void SetWheelsPower(int8_t ref_power_l, int8_t ref_power_r);
  void StopWheels(bool brake);

  int32_t counts_l_;
  int32_t counts_r_;
  int8_t power_l_;
  int8_t power_r_;

 private:
  void ResetCounts();
};

class SensorIo {
 public:
  SensorIo();
  ~SensorIo();
  void Update();

  bool touch_sensor_pressed_;
  bool back_button_pressed_;
  rgb_raw_t color_rgb_raw_;
};

#endif  // ETRC21_DEVICE_IO_H_
