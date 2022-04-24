#include "Tetrimino.h"

namespace Tetris
{
	Tetrimino::Tetrimino() : Polyomino<4>()
	{
	}

	Tetrimino::Tetrimino(glm::vec4 color, std::array<glm::ivec2, 4> positions, glm::ivec2 starting_pos) : Polyomino<4>(color, positions, starting_pos)
	{
	}

	Tetrimino::TetriminoLetter Tetrimino::getLetter()
	{
		return this->my_letter;
	}

	Tetrimino::RotationState Tetrimino::getRotationState()
	{
		return this->rotation_state;
	}

	void Tetrimino::rotate(bool clockwise)
	{
		if (clockwise)
		{
			this->rotateClockwise();
		}
		else
		{
			this->rotateCounterClockwise();
		}
	}

	void Tetrimino::rotateClockwise()
	{
		this->rotation_state++;
		glm::ivec2 kick_0 = this->offset_data[static_cast<int>(this->rotation_state)][0];
		glm::ivec2 pivot = this->monominoes[0]->getMainOffset();

		for (int i = 0; i < Tetrimino::N; i++)
		{
			glm::ivec2 offset = this->monominoes[i]->getMainOffset();
			glm::ivec2 rotation = this->monominoes[i]->getRotation();
			glm::ivec2 current_pos = offset + rotation;
			glm::ivec2 normalized_pos = current_pos - pivot;
			glm::ivec2 new_rotated_pos = glm::ivec2(-normalized_pos.y, normalized_pos.x);
			glm::ivec2 new_rotation = new_rotated_pos - offset + pivot;
			this->monominoes[i]->setRotation(new_rotation.x, new_rotation.y);
		}
	}

	void Tetrimino::rotateCounterClockwise()
	{
		this->rotation_state--;
		glm::ivec2 pivot = this->monominoes[0]->getMainOffset();
		glm::ivec2 kick_0 = this->offset_data[static_cast<int>(this->rotation_state)][0];

		for (int i = 0; i < Tetrimino::N; i++)
		{
			glm::ivec2 offset = this->monominoes[i]->getMainOffset();
			glm::ivec2 rotation = this->monominoes[i]->getRotation();
			glm::ivec2 current_pos = offset + rotation;
			glm::ivec2 normalized_pos = current_pos - pivot;
			glm::ivec2 new_rotated_pos = glm::ivec2(normalized_pos.y, -normalized_pos.x);
			glm::ivec2 new_rotation = new_rotated_pos - offset + pivot;
			this->monominoes[i]->setRotation(new_rotation.x, new_rotation.y);
		}
	}

	glm::vec2 Tetrimino::getPreviewPos()
	{
		return this->preview_pos;
	}

	void Tetrimino::kick(int index, bool clockwise)
	{
		if (index >= 5)
		{
			std::cerr << "[TETRIS]: Kick index out of bounds: " << index << std::endl;
			return;
		}

		RotationState old_rot = this->rotation_state;
		RotationState new_rot = this->rotation_state;

		if (clockwise)
		{
			old_rot--;
		}
		else
		{
			old_rot++;
		}

		glm::ivec2 kick_pre = this->offset_data[static_cast<int>(old_rot)][index];
		glm::ivec2 kick_post = this->offset_data[static_cast<int>(new_rot)][index];
		glm::ivec2 kick = kick_post - kick_pre;

		for (int i = 0; i < Tetrimino::N; i++)
		{
			this->monominoes[i]->setKick(kick.x, kick.y);
		}
	}

	void Tetrimino::kickToPos()
	{
		glm::ivec2 kick = glm::ivec2(0, 0);
		for (int i = 0; i < Tetrimino::N; i++)
		{
			kick = this->monominoes[i]->getKick();
			this->monominoes[i]->moveFor(kick.x, kick.y);
			this->monominoes[i]->setKick(0, 0);
		}
		this->setPos(this->getPos() + kick);
	}

	void Tetrimino::resetKick()
	{
		for (int i = 0; i < Tetrimino::N; i++)
		{
			this->monominoes[i]->setKick(0, 0);
		}
	}

	Tetrimino::RotationState& operator++(Tetrimino::RotationState& rot, int)
	{
		switch (rot)
		{
		case Tetrimino::RotationState::ROTATION_NONE:
			return rot = Tetrimino::RotationState::ROTATION_ONCE_CLOCKWISE;
		case Tetrimino::RotationState::ROTATION_ONCE_CLOCKWISE:
			return rot = Tetrimino::RotationState::ROTATION_DOUBLE;
		case Tetrimino::RotationState::ROTATION_DOUBLE:
			return rot = Tetrimino::RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE;
		case Tetrimino::RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE:
			return rot = Tetrimino::RotationState::ROTATION_NONE;
		}

		return rot = Tetrimino::RotationState::ROTATION_NONE;
	};

	Tetrimino::RotationState& operator--(Tetrimino::RotationState& rot, int)
	{
		switch (rot)
		{
		case Tetrimino::RotationState::ROTATION_NONE:
			return rot = Tetrimino::RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE;
		case Tetrimino::RotationState::ROTATION_ONCE_CLOCKWISE:
			return rot = Tetrimino::RotationState::ROTATION_NONE;
		case Tetrimino::RotationState::ROTATION_DOUBLE:
			return rot = Tetrimino::RotationState::ROTATION_ONCE_CLOCKWISE;
		case Tetrimino::RotationState::ROTATION_ONCE_COUNTER_CLOCKWISE:
			return rot = Tetrimino::RotationState::ROTATION_DOUBLE;
		}

		return rot = Tetrimino::RotationState::ROTATION_NONE;
	};
}
