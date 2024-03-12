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
	WHITE_SHADING,
	FLAT,
	GOURAUD,
	PHONG
};

namespace cg {
	namespace constants {
		// default width and height values
		constexpr unsigned int SCR_WIDTH = 1200;
		constexpr unsigned int SCR_HEIGHT = 900;
	} // namespace constants
} // namespace cg