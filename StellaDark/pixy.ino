int16_t angle_goal() {
	static uint8_t no_goal_cnt = 0;
	static int16_t last_goal_angle = GOAL_ANGLE_NO_GOAL;
	int16_t goal_angle;
	pixyViSy.update(PIXYVISY_GOAL | PIXYVISY_ANGG);

	if (pixyViSy.getGoalAction() == 'N') {
		if (no_goal_cnt > 15) {
			goal_angle = GOAL_ANGLE_NO_GOAL;
		} else {
			goal_angle = last_goal_angle;
			no_goal_cnt++;
		}
	} else {
		goal_angle = pixyViSy.getGoalAngle();
		last_goal_angle = goal_angle;
		no_goal_cnt = 0;
	}
	return goal_angle;
}

int robot_angle() {
	if (defend) {
		return compass();
	} else {
		static int last_relevant_angle = 0;
		int goal_angle;
		goal_angle = angle_goal();
		if (goal_angle == GOAL_ANGLE_NO_GOAL) {
			return compass() - last_relevant_angle;
		} else {
			last_relevant_angle = compass();
			return -goal_angle;
		}
	}
}

int16_t team_distance() {
	int16_t distance;
	static long long last_time;
	static long long now_time;
	distance = pixyViSy.getTeamDist();
	now_time = millis();
	if (distance == PIXYVISY_NOBLOCK) {
		if ((now_time - last_time) < MAX_NO_TEAM_TIME) {
			return last_distance;
		} else {
			return PIXYVISY_NOBLOCK;
		}
	} else {
		last_distance = distance;
		last_time = now_time;
		return distance;
	}
}

int16_t team_angle() {
	int16_t angle;
	int16_t last_angle;
	angle = pixyViSy.getTeamAngle();
	if (pixyViSy.getTeamDist() == PIXYVISY_NOBLOCK) {
		return last_angle;
	} else {
		last_angle = angle;
		return angle;
	}
}

int16_t team_Dx() {
	int16_t Dx;
	int16_t last_Dx;
	Dx = pixyViSy.getTeamDx();
	if (pixyViSy.getTeamDist() == PIXYVISY_NOBLOCK) {
		return last_Dx;
	} else {
		last_Dx = Dx;
		return Dx;
	}
}
