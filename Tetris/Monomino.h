#pragma once
#include "Rectangle.h"
#include "ShaderProgram.h"
#include "Playfield.h"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace Tetris
{
	class Monomino
	{
	public:
		Monomino();
		Monomino(glm::vec4 color);
		void draw(std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Playfield> playfield);
		void draw(std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Playfield> playfield, glm::vec2 render_pos);
		void moveTo(int x, int y);
		void moveFor(int x, int y);
		void setColor(glm::vec4 color);
		void setAsGhost();
		glm::ivec2 getMainPos();
		glm::ivec2 getFinalPos();
		void setOffset(int x, int y);
		glm::ivec2 getMainOffset();
		glm::ivec2 getFinalOffset();
		void setRotation(int x, int y);
		glm::ivec2 getRotation();
		void setKick(int x, int y);
		glm::ivec2 getKick();
		void destroy();
	private:
		bool initialized = false;
		int x = 5;	//all monominoes in a polyomino should have the same x/y
		int y = 20;
		int x_offset = 0;	//offset defining polyomino's shape
		int y_offset = 0;
		int x_rotation = 0;	//rotation after shape offset is applied
		int y_rotation = 0;
		int x_kick = 0;
		int y_kick = 0;
		float render_scale = 1.0f;
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Geometry::Rectangle render_square;
		void initDrawable(float size);
	};
}
