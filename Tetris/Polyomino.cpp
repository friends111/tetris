#include "Polyomino.h"

namespace Tetris
{
	template<unsigned int N>
	Polyomino<N>::Polyomino()
	{
		for (int i = 0; i < N; i++)
		{
			std::shared_ptr<Monomino> monomino = std::make_shared<Monomino>();
			this->monominoes[i] = monomino;
		}
	}

	template<unsigned int N>
	Polyomino<N>::Polyomino(glm::vec4 color, std::array<glm::ivec2, N> positions, glm::ivec2 starting_pos)
	{
		this->x = starting_pos.x;
		this->y = starting_pos.y;

		for (int i = 0; i < N; i++)
		{
			std::shared_ptr<Monomino> monomino = std::make_shared<Monomino>(color);
			//monomino->setColor(color * (0.6f + ((float)i/(float)N)/3.0f) );
			monomino->moveTo(starting_pos.x, starting_pos.y);
			monomino->setOffset(positions[i].x, positions[i].y);
			this->monominoes[i] = monomino;
		}
	}

	/*
	template<unsigned int N>
	void Polyomino<N>::draw(ShaderProgram* shader_program, Playfield* playfield)
	{
		for (std::shared_ptr<Monomino> m : this->monominoes)
		{
			m->draw(shader_program, playfield);
		}
	}
	*/

	template<unsigned int N>
	std::vector<std::shared_ptr<Monomino>> Polyomino<N>::getMonominoes()
	{
		return std::vector<std::shared_ptr<Monomino>>(this->monominoes.begin(), this->monominoes.end());
	}

	template<unsigned int N>
	void Polyomino<N>::moveTo(int x, int y)
	{
		this->x = x;
		this->y = y;

		for (int i = 0; i < N; i++)
		{
			this->monominoes[i]->moveTo(x, y);
		}
	}

	template<unsigned int N>
	void Polyomino<N>::moveFor(int x, int y)
	{
		this->x += x;
		this->y += y;

		for (int i = 0; i < N; i++)
		{
			this->monominoes[i]->moveFor(x, y);
		}
	}

	template<unsigned int N>
	void Polyomino<N>::setAsGhost()
	{
		for (int i = 0; i < N; i++)
		{
			this->monominoes[i]->setAsGhost();	//TODO
		}
	}

	template<unsigned int N>
	glm::ivec2 Polyomino<N>::getPos()
	{
		return glm::ivec2(this->x, this->y);
	}

	template<unsigned int N>
	void Polyomino<N>::setPos(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	template<unsigned int N>
	void Polyomino<N>::setPos(glm::ivec2 pos)
	{
		this->x = pos.x;
		this->y = pos.y;
	}

	template Polyomino<4>;	//Tetrimino
}