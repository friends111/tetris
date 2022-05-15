#pragma once
#include <glm/vec2.hpp>
#include "ShaderProgram.h"
#include "Rectangle.h"
#include <vector>
#include <memory>

namespace Tetris
{
	class Playfield
	{
	private:
		const int width = 10;
		const int height = 40;
		const int playable_width_offset = 0;
		const int playable_height_offset = 20;
		const int screen_width;
		const int screen_height;
		const float cell_size = 32.0f; // one grid cell is a square with length cell_size pixels
		const float cell_spacing = 1.0f;
		Geometry::Rectangle board_background;
		Geometry::Rectangle board_background_border;
		float board_top_boundary = 0.0f;
		Geometry::Rectangle hold_background;
		Geometry::Rectangle hold_background_border;
		Geometry::Rectangle queue_background;
		Geometry::Rectangle queue_background_border;
		Geometry::Rectangle score_background;
		Geometry::Rectangle score_background_border;
		std::vector<std::vector<std::unique_ptr<Geometry::Rectangle>>> grid_squares;
		int getPlayableWidth();
		int getPlayableHeight();
		void initGridDrawables();
	public:
		Playfield(int screen_width, int screen_height);
		glm::vec2 gridToScreenCoordinates(glm::ivec2 grid_pos);
		glm::vec2 gridToScreenCoordinates(int x, int y);
		float getCellSize();
		float getBoardTopBoundary();
		glm::ivec2 getSize();
		void draw(std::shared_ptr<OpenGLWrapper::ShaderProgram> shader_program);
	};
}
