// constants.h
#pragma once
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
namespace cg {
	namespace constants {
		// default width and height values
		constexpr unsigned int SCR_WIDTH = 800;
		constexpr unsigned int SCR_HEIGHT = 600;
	} // namespace constants
} // namespace cg