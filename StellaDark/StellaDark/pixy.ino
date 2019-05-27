
uint16_t goal_distance, ball_distance;
int16_t goal_angle;
char action;
uint32_t time, loop_count = 0;
uint16_t min_time = ~0, max_time = 0, average_time = 0;

int16_t angle_goal() {
  static uint8_t no_goal_cnt = 0;
  static int16_t last_goal_angle = GOAL_ANGLE_NO_GOAL;
  pixyViSy.update(2);

  if (pixyViSy.getBallDist() == ~(uint16_t)0) {
    if (no_goal_cnt > 15) {
      goal_angle = GOAL_ANGLE_NO_GOAL;
    } else {
      goal_angle = last_goal_angle;
      no_goal_cnt++;
    }
  } else {
    goal_angle = pixyViSy.getBallAngle();
    last_goal_angle = goal_angle;
    no_goal_cnt = 0;
  }
  return goal_angle;
}

int robot_angle() {
  if(defend){
    return compass();
  }else{
  static int last_relevant_angle = 0;
  int goal_angle;
  goal_angle = angle_goal();
  if ( goal_angle == GOAL_ANGLE_NO_GOAL) {
    return compass() - last_relevant_angle;
  }
  else {
    last_relevant_angle = compass();
    return -goal_angle;
  }
    }
}
bool ColorCode() {
  pixyViSy.update(3);
  return pixyViSy.getColorCode();
}
