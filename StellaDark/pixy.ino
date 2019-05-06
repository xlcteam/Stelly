
uint16_t goal_distance, ball_distance;
int8_t goal_angle;
char action;
uint32_t time, loop_count = 0;
uint16_t min_time = ~0, max_time = 0, average_time = 0;

int angle_goal() {
  static int no_goal = 0;
  static int last_goal_angle = 0;
  pixyViSy.update();
  goal_angle = pixyViSy.getBallAngle();
  if (goal_angle == 0) {
    no_goal++;
    if (no_goal > 15) {
      goal_angle = 0;
      return 255;
    }
    else {
      goal_angle = last_goal_angle;
    }
  }
  else {
    no_goal = 0;
    last_goal_angle = goal_angle;
  }
  return goal_angle;
}

int robot_angle() {
  static int last_relevant_angle = 0;
  int goal_angle;
  goal_angle = angle_goal();
  if ( goal_angle == 255) {
    return compass() - last_relevant_angle;
  }
  else {
    last_relevant_angle = compass();
    return -goal_angle;
  }
}

