#include "Monomino.h"

namespace Tetris
{
	Monomino::Monomino()
	{
	}

	Monomino::Monomino(glm::vec4 color)
	{
		this->color = color;
	}

	void Monomino::draw(std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Playfield> playfield)
	{
		glm::ivec2 final_pos = this->getFinalPos();
		glm::vec2 render_pos = playfield->gridToScreenCoordinates(final_pos.x, final_pos.y);
		this->draw(shader_program, playfield, render_pos);
	}

	void Monomino::draw(std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Playfield> playfield, glm::vec2 render_pos)
	{
		if (!this->initialized)
		{
			this->initDrawable(playfield->getCellSize());
		}

		this->render_square.setScale(this->render_scale, this->render_scale, this->render_scale);
		this->render_square.setPos(render_pos.x, render_pos.y);
		this->render_square.setColor(this->color);
		this->render_square.draw(shader_program);
	}

	void Monomino::moveTo(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void Monomino::moveFor(int x, int y)
	{
		this->x += x;
		this->y += y;
	}

	void Monomino::setColor(glm::vec4 color)
	{
		this->color = color;
	}

	void Monomino::setAsGhost()
	{
		this->color *= glm::vec4(1.0, 1.0, 1.0, 0.3);	//TODO
	}

	glm::ivec2 Monomino::getMainPos()
	{
		return glm::ivec2(this->x, this->y);
	}

	glm::ivec2 Monomino::getFinalPos()
	{
		int final_x = this->x + this->x_offset + this->x_rotation + this->x_kick;
		int final_y = this->y + this->y_offset + this->y_rotation + this->y_kick;
		return glm::ivec2(final_x, final_y);
	}

	void Monomino::setOffset(int x, int y)
	{
		this->x_offset = x;
		this->y_offset = y;
	}

	glm::ivec2 Monomino::getMainOffset()
	{
		return glm::ivec2(this->x_offset, this->y_offset);
	}

	glm::ivec2 Monomino::getFinalOffset()
	{
		return this->getFinalPos() - this->getMainPos();
	}

	void Monomino::setRotation(int x, int y)
	{
		this->x_rotation = x;
		this->y_rotation = y;
	}

	glm::ivec2 Monomino::getRotation()
	{
		return glm::ivec2(this->x_rotation, this->y_rotation);
	}

	void Monomino::setKick(int x, int y)
	{
		this->x_kick = x;
		this->y_kick = y;
	}

	glm::ivec2 Monomino::getKick()
	{
		return glm::ivec2(this->x_kick, this->y_kick);
	}

	void Monomino::destroy()
	{
		this->render_square.destroy();
	}

	void Monomino::initDrawable(float size)
	{
		this->render_square.initFrom2D(glm::vec2(-0.5f, -0.5f), glm::vec2(0.5f, 0.5f));
		this->render_scale = size;
		this->initialized = true;
	}
}
