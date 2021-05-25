#ifndef ETRC21_UTILS_H_
#define ETRC21_UTILS_H_

class PidControl {
 public:
  PidControl();
  void SetGain(float kp, float ki, float kd);
  float GetMv(float target_val, float current_val);

 private:
  float kp_;
  float ki_;
  float kd_;
  float dt_;
  float diff0_;
  float diff1_;
  float integral_;
};

#endif  // ETRC21_UTILS_H_
