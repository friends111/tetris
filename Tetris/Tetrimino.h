#pragma once
#include "Polyomino.h"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <array>
#include "MathHelper.h"

namespace Tetris
{
	using MathHelper::Point;

	class Tetrimino : public Polyomino<4>
	{
	public:
		enum class TetriminoLetter : unsigned int
		{
			TETRIMINO_NONE = 0,
			TETRIMINO_I = 1,
			TETRIMINO_L = 2,
			TETRIMINO_J = 3,
			TETRIMINO_S = 4,
			TETRIMINO_Z = 5,
			TETRIMINO_O = 6,
			TETRIMINO_T = 7,
		};
		enum class RotationState : unsigned int
		{
			ROTATION_NONE = 0,	//0
			ROTATION_ONCE_CLOCKWISE = 1,	//R
			ROTATION_DOUBLE = 2,	//2
			ROTATION_ONCE_COUNTER_CLOCKWISE = 3,	//L
		};
		Tetrimino();
		Tetrimino(glm::vec4 color, std::array<glm::ivec2, 4> positions, glm::ivec2 starting_pos);
		TetriminoLetter getLetter();
		RotationState getRotationState();
		void rotate(bool clockwise);
		glm::vec2 getPreviewPos();
		void kick(int index, bool clockwise = true);
		void kickToPos();
		void resetKick();
	protected:
		const static int N = 4;
		TetriminoLetter my_letter = TetriminoLetter::TETRIMINO_NONE;
		RotationState rotation_state = RotationState::ROTATION_NONE;
		std::array<std::array<Point, 5>, 4> offset_data;
		glm::vec2 preview_pos = glm::vec2(0, 0);
		int current_kick_x = 0;
		int current_kick_y = 0;
	private:
		friend RotationState& operator++(RotationState& rot, int);
		friend RotationState& operator--(RotationState& rot, int);
		void rotateClockwise();
		void rotateCounterClockwise();
	};

	class TetriminoI : public Tetrimino
	{
	public:
		TetriminoI() : Tetrimino(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), { { { 2, 2 }, { 1, 2 }, { 3, 2 }, { 4, 2 } } }, glm::ivec2(2, 18))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_I;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(-1.5f, -1.5f);
		}
	};

	class TetriminoO : public Tetrimino
	{
	public:
		TetriminoO() : Tetrimino(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), { { { 1, 1 }, { 1, 0 }, { 2, 0 }, { 2, 1 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_O;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, -1}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {-1, -1}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { { -1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(-0.5f, -.0f);
		}
	};

	class TetriminoT : public Tetrimino
	{
	public:
		TetriminoT() : Tetrimino(glm::vec4(0.75f, 0.0f, 1.0f, 1.0f), { { { 1, 1 }, { 0, 1 }, { 1, 0 }, { 2, 1 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_T;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(0.0f, 0.0f);
		}
	};

	class TetriminoS : public Tetrimino
	{
	public:
		TetriminoS() : Tetrimino(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), { { { 1, 1 }, { 0, 1 }, { 1, 0 }, { 2, 0 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_S;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(0.0f, 0.0f);
		}
	};

	class TetriminoZ : public Tetrimino
	{
	public:
		TetriminoZ() : Tetrimino(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), { { { 1, 1 }, { 0, 0 }, { 1, 0 }, { 2, 1 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_Z;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(0.0f, 0.0f);
		}
	};

	class TetriminoJ : public Tetrimino
	{
	public:
		TetriminoJ() : Tetrimino(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), { { { 1, 1 }, { 0, 0 }, { 0, 1 }, { 2, 1 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_J;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(0.0f, 0.0f);
		}
	};

	class TetriminoL : public Tetrimino
	{
	public:
		TetriminoL() : Tetrimino(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), { { { 1, 1 }, { 0, 1 }, { 2, 1 }, { 2, 0 } } }, glm::ivec2(3, 20))
		{
			this->my_letter = TetriminoLetter::TETRIMINO_L;
			this->offset_data[static_cast<int>(RotationState::ROTATION_NONE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_CLOCKWISE)] = { { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_DOUBLE)] = { { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} } };
			this->offset_data[static_cast<int>(RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE)] = { { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } };
			for (auto& a : this->offset_data)
			{
				for (auto& p : a)
				{
					p.x *= -1;
				}
			}
			this->preview_pos = glm::vec2(0.0f, 0.0f);
		}
	};
}
