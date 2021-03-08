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
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
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

vec2 mouse_in_world_coord(vec2 mouse_pos)
{
	auto view = registry.view<Motion>();
	auto& camera_motion = view.get<Motion>(camera);
	vec2 mouse_world_pos = vec2({ (mouse_pos.x + camera_motion.position.x) / camera_motion.scale.x,
								  (mouse_pos.y + camera_motion.position.y) / camera_motion.scale.y });
	return mouse_world_pos;
}

vec2 coord_to_pixel(ivec2 grid_coord) {
    return grid_coord * GRID_CELL_SIZE + GRID_OFFSET;
}

ivec2 pixel_to_coord(vec2 pixel_position) {
    return (ivec2)pixel_position / GRID_CELL_SIZE;
}

// convert original scale to some set multiple of cell units
vec2 scale_to_grid_units(vec2 original_scale, float cell_units) {
    vec2 unit_scale = original_scale / min(original_scale.x, original_scale.y);
    return unit_scale * cell_units * (float) GRID_CELL_SIZE;
}

// unit_velocity: velocity as a multiple of grid cell units
// eg. unit_velocity = vec2(1,1) means moving 1 grid right and 1 grid down every second
vec2 grid_to_pixel_velocity(vec2 unit_velocity) {
    return unit_velocity * (vec2) GRID_CELL_SIZE;
}

bool is_inbounds(ivec2 grid_coord) {
    return grid_coord.x >= 0 && grid_coord.y >= 0 && grid_coord.x < MAP_SIZE_IN_COORD.x && grid_coord.y < MAP_SIZE_IN_COORD.y;
}
