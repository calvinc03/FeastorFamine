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
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x + camera_position.x, offset.y + camera_position.y, 1.f } };
	mat = mat * T;
}

void Transform::move_camera(vec2 offset)
{
	camera_position.x += offset.x;
	camera_position.y += offset.y;
}



//detects if mouse is within the a rectangle of size scale at position entity_pos
//not easy to use right now, look at how it's used in the ui procedures.
float sdBox(vec2 mouse_pos_grid, vec2 entity_pos, vec2 scale) {
	vec2 d = abs(mouse_pos_grid - entity_pos) - scale;
	return length(max(d, vec2(0.0, 0.0))) + min(max(d.x, d.y), 0.0f);
}


entt::registry registry;

entt::entity screen_state_entity;
// for camera view; zoom & pan
entt::entity camera;
