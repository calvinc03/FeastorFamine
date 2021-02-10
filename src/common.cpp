#include "common.hpp"

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = std::cos(radians);
	float s = std::sin(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x * GRID_CELL_SIZE + camera_position.x, offset.y * GRID_CELL_SIZE + camera_position.y, 1.f } };
	mat = mat * T;
}

void Transform::move_camera(vec2 offset)
{
	camera_position.x += offset.x;
	camera_position.y += offset.y;
}


std::string button_to_string(int button) {
	switch (button) {
	case tower_button:
		return "tower_button";
	case green_house_button:
		return "green_house_button";
	case stick_figure_button:
		return "stick_figure_button";
	}
	return "no button / invalid button / or this method is broken!";
}


entt::registry registry;

entt::entity screen_state_entity;
// for camera view; zoom & pan
entt::entity camera;
