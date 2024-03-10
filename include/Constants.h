// Constants.h
#pragma once
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ORBIT_L,
	ORBIT_R
};

enum ShadingType {
	FLAT,
	GOURAUD,
	PHONG
};

namespace cg {
	namespace constants {
		// default width and height values
		constexpr unsigned int SCR_WIDTH = 800;
		constexpr unsigned int SCR_HEIGHT = 600;
	} // namespace constants
} // namespace cg