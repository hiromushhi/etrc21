#include "device_io.h"

MotorIo::MotorIo() {
  ev3_motor_config(EV3_PORT_B, LARGE_MOTOR);
  ev3_motor_config(EV3_PORT_C, LARGE_MOTOR);
}

MotorIo::~MotorIo() {
}

void MotorIo::Update() {
  counts_r_ = ev3_motor_get_counts(EV3_PORT_B);
  counts_l_ = ev3_motor_get_counts(EV3_PORT_C);
}

SensorIo::SensorIo() {
  ev3_sensor_config(EV3_PORT_1, TOUCH_SENSOR);
  ev3_sensor_config(EV3_PORT_2, COLOR_SENSOR);
}

SensorIo::~SensorIo() {
}

void SensorIo::Update() {
  touch_pressed_ = ev3_touch_sensor_is_pressed(EV3_PORT_1);
  ev3_color_sensor_get_rgb_raw(EV3_PORT_2, &color_rgb_raw_);
}
