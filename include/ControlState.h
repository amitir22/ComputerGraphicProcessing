#pragma once

struct ControlState {
	float last_x;
	float last_y;
	bool first_mouse;
	float delta_time;
	float last_frame;

	ControlState(float initial_width, float initial_height);
	void UpdateDeltaTime(float current_frame);

};