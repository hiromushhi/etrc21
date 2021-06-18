#include "app.h"

#include "etroboc_ext.h"
#include "device_io.h"
#include "etrc_info.h"
#include "driving.h"
#include "game_play.h"
#include "state.h"

#if defined(MAKE_RIGHT)
static const bool kRcourse = true;
#else
static const bool kRcourse = false;
#endif

MotorIo* motor_io;
SensorIo* sensor_io;
Luminous* luminous;
Localize* localize;
WheelsControl* wheels_control;
LineTracer* line_tracer;
BasicMover* basic_mover;
EndCondition* end_condition;
DrivingManager* driving_manager;
BingoAgent* bingo_agent;
StateManager* state_manager;

static void initialize() {
  motor_io = new MotorIo();
  sensor_io = new SensorIo();
  luminous = new Luminous(sensor_io);
  localize = new Localize(motor_io);
  wheels_control = new WheelsControl(motor_io);
  line_tracer = new LineTracer(wheels_control, luminous);
  basic_mover = new BasicMover(wheels_control);
  end_condition = new EndCondition(luminous, localize);
  driving_manager = new DrivingManager(line_tracer, basic_mover, end_condition);
  bingo_agent = new BingoAgent(kRcourse);
  state_manager = new StateManager(driving_manager, bingo_agent, kRcourse);
}

static void finalize() {
  delete state_manager;
  delete bingo_agent;
  delete driving_manager;
  delete end_condition;
  delete basic_mover;
  delete line_tracer;
  delete wheels_control;
  delete localize;
  delete luminous;
  delete sensor_io;
  delete motor_io;
}

static void setup_blockbingo() {
  tslp_tsk(1000*1000U);
  char k1 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_BLACK1));
  char r1 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_RED1));
  char r2 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_RED2));
  char y1 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_YELLOW1));
  char y2 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_YELLOW2));
  char b1 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_BLUE1));
  char b2 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_BLUE2));
  char g1 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_GREEN1));
  char g2 = static_cast<char>(ETRoboc_getCourseInfo(ETROBOC_COURSE_INFO_BLOCK_POS_GREEN2));

  bingo_agent->SetBlockPos(kK1, k1);
  bingo_agent->SetBlockPos(kR1, r1);
  bingo_agent->SetBlockPos(kR2, r2);
  bingo_agent->SetBlockPos(kY1, y1);
  bingo_agent->SetBlockPos(kY2, y2);
  bingo_agent->SetBlockPos(kB1, b1);
  bingo_agent->SetBlockPos(kB2, b2);
  bingo_agent->SetBlockPos(kG1, g1);
  bingo_agent->SetBlockPos(kG2, g2);
}

void main_task(intptr_t unused) {
  initialize();

  sta_cyc(UPDATE_INFO_CYC);
  while (true) {
    if (sensor_io->touch_sensor_pressed_) {
      break;
    }
    tslp_tsk(10*1000U);
  }

  sta_cyc(EXEC_ACTION_CYC);
  setup_blockbingo();
  while (true) {
    if (sensor_io->back_button_pressed_) {
      break;
    }
    tslp_tsk(100*1000U);
  }

  stp_cyc(EXEC_ACTION_CYC);
  stp_cyc(UPDATE_INFO_CYC);
  finalize();
  ext_tsk();
}

void exec_action_task(intptr_t unused) {
  state_manager->Update();
  ext_tsk();
}

void update_info_task(intptr_t unused) {
  motor_io->Update();
  sensor_io->Update();
  luminous->Update();
  localize->Update();
  ext_tsk();
}
