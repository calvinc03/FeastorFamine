#pragma once


#include "entt.hpp"
#include "common.hpp"
#include "render.hpp"
#include "text.hpp"
#include <vector>
#include <string>
#include <json.hpp>



std::string button_to_string(int button); // breaks if enums change
Button UI_click_system();
void UI_highlight_system(vec2 mouse_pos);

entt::entity create_ui_text(vec2 position, std::string content, float scale = 0.3f, vec3 colour = { 1.0f,1.0f,1.0f });

struct UI_element {
	std::string tag;
	vec2 position;
	vec2 scale;
	float angle = 0;
};

struct UI_background
{
	static entt::entity createUI_background();
	static entt::entity createUI_top_bar();
};

struct UI_button
{
	static entt::entity createUI_build_unit_button(int pos, Button button, size_t cost = 0, std::string tag = "default_ui_button_tag", bool show = true);
	static entt::entity createUI_button(int pos, Button button, std::string tag = "default_ui_button_tag", bool show = true);
	// wrapper for buttons that shows up when a unit is selected
	static entt::entity createUI_selected_unit_button(int pos, Button button, std::string tag = "default_ui_button_tag", bool show = true);
};

struct UI_banner
{
	static entt::entity createUI_Banner(std::string content, vec2 position, vec3 colour, float duration);
};

struct UI_selected_unit_portrait
{
	static entt::entity createUI_selected_unit_portrait(unit_type type);
};

inline std::string ui_texture_path(const std::string& name) { return data_path() + "/textures/ui/" + name; };

void change_button_text(entt::entity button_entity, std::string button_text);

void aligne_text_right(entt::entity entity, float right_alignment_position);

struct UI_season_wheel {
	static entt::entity createUI_season_wheel();
	static entt::entity createUI_season_wheel_arrow();
	static void get_season_sequence();
};

struct UI_weather_icon {
	static entt::entity createUI_weather_icon();
	static void change_weather_icon(entt::entity entity, int weather);
};

struct UI_selected_unit
{

};

struct UI_build_unit
{
	static void fill_UI_build_unit_component(UI_build_unit& ui_build_unit, Button button);
	std::string unit_name = "";
	std::vector<std::string> descriptions = {};
};

struct UI_unit_stats
{

};

