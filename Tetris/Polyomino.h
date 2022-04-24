#pragma once
#include "Monomino.h"
#include "ShaderProgram.h"
#include <type_traits>
#include <array>
#include <vector>
#include "Monomino.h"
#include <memory>
#include <glm/vec2.hpp>

namespace Tetris
{
	template<unsigned int N>
	class Polyomino
	{
		static_assert(N > 0, "Polyomino<N> : N must be higher than 0.");
	protected:
		int x = 0;
		int y = 0;
		std::array<std::shared_ptr<Monomino>, N> monominoes;
	public:
		Polyomino();
		Polyomino(glm::vec4 color, std::array<glm::ivec2, N> positions, glm::ivec2 starting_pos);
		std::vector<std::shared_ptr<Monomino>> getMonominoes();
		//void draw(ShaderProgram* shader_program, Playfield* playfield);
		void moveTo(int x, int y);
		void moveFor(int x, int y);
		void setAsGhost();
		glm::ivec2 getPos();
		void setPos(int x, int y);
		void setPos(glm::ivec2 pos);
	};
}
