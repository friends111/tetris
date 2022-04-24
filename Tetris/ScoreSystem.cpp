#include "ScoreSystem.h"

namespace Tetris
{
	ScoreSystem::ScoreSystem(MessageSystem& message_system) : message_system(message_system)
	{

	}

	int ScoreSystem::getCombo()
	{
		return this->combo;
	}

	void ScoreSystem::resetCombo()
	{
		this->combo = -1;
	}

	void ScoreSystem::increaseCombo()
	{
		this->combo++;
	}

	int ScoreSystem::getTotalLinesCleared()
	{
		return this->total_lines_cleared;
	}

	void ScoreSystem::resetTotalLinesCleared()
	{
		this->total_lines_cleared = 0;
	}

	void ScoreSystem::increaseTotalLinesCleared()
	{
		this->total_lines_cleared++;
	}

	int ScoreSystem::getTotalTetrises()
	{
		return this->total_tetrises;
	}

	void ScoreSystem::resetTotalTetrises()
	{
		this->total_tetrises = 0;
	}

	void ScoreSystem::increaseTotalTetrises()
	{
		this->total_tetrises++;
	}

	float ScoreSystem::getTetrisRate()
	{
		if (this->total_lines_cleared <= 0)
		{
			return 0.0f;
		}

		return (100.0f * 4.0f * this->total_tetrises) / static_cast<float>(this->total_lines_cleared);
	}

	int ScoreSystem::getPoints()
	{
		return this->points;
	}

	void ScoreSystem::resetPoints()
	{
		this->points = 0;
	}

	void ScoreSystem::addPoints(int level, int amount, std::string reason, bool multiply_by_level, bool back_to_back, bool print)
	{
		int final_amount = multiply_by_level ? amount * level : amount;
		if (back_to_back)
		{
			final_amount = static_cast<int>(final_amount * this->back_to_back_multiplier);
		}

		this->points += final_amount;

		if (back_to_back)
		{
			reason += " (Back to Back)";
			this->message_system.addMessage(*S::TETRIS_BACK_TO_BACK);
		}

		if (print)
		{
			std::cout << "[TETRIS]: New points! +" << final_amount << " (" << reason << "). Total: " << this->points << std::endl;
		}
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::CHEATING>()
	{
		this->addPoints(0, 1, "Cheating", false, false);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::SOFT_DROP>()
	{
		this->addPoints(0, this->points_soft_drop_per_cell, "Soft Drop", false, false, false);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::HARD_DROP>()
	{
		this->addPoints(0, this->points_hard_drop_per_cell, "Hard Drop", false, false, false);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::SINGLE>(int level)
	{
		this->addPoints(level, this->points_single, "Single", true, false);
		this->message_system.addMessage(*S::TETRIS_SINGLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::DOUBLE>(int level)
	{
		this->addPoints(level, this->points_double, "Double", true, false);
		this->message_system.addMessage(*S::TETRIS_DOUBLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::TRIPLE>(int level)
	{
		this->addPoints(level, this->points_triple, "Triple", true, false);
		this->message_system.addMessage(*S::TETRIS_TRIPLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::TETRIS>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_tetris, "Tetris", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_TETRIS);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_MINI>(int level)
	{
		this->addPoints(level, this->points_t_spin_mini, "T-Spin Mini", true, false);
		this->message_system.addMessage(*S::TETRIS_T_SPIN_MINI);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN>(int level)
	{
		this->addPoints(level, this->points_t_spin, "T-Spin", true, false);
		this->message_system.addMessage(*S::TETRIS_T_SPIN);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::COMBO>(int level)
	{
		this->addPoints(level, static_cast<int>(this->combo * this->combo_multiplier), "Combo", true, false);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_MINI_SINGLE>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_t_spin_mini_single, "T-Spin Mini Single", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_T_SPIN_MINI + " " + *S::TETRIS_SINGLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_MINI_DOUBLE>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_t_spin_mini_double, "T-Spin Mini Double", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_T_SPIN_MINI + " " + *S::TETRIS_DOUBLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_SINGLE>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_t_spin_single, "T-Spin Single", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_T_SPIN + " " + *S::TETRIS_SINGLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_DOUBLE>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_t_spin_double, "T-Spin Double", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_T_SPIN + " " + *S::TETRIS_DOUBLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::T_SPIN_TRIPLE>(int level, bool back_to_back)
	{
		this->addPoints(level, this->points_t_spin_triple, "T-Spin Triple", true, back_to_back);
		this->message_system.addMessage(*S::TETRIS_T_SPIN + " " + *S::TETRIS_TRIPLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::PERFECT_CLEAR_SINGLE>(int level)
	{
		this->addPoints(level, this->perfect_clear_single, "Perfect Clear (Single)", true, false);
		this->message_system.addMessage(*S::TETRIS_PERFECT_CLEAR);
		this->message_system.addMessage(*S::TETRIS_SINGLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::PERFECT_CLEAR_DOUBLE>(int level)
	{
		this->addPoints(level, this->perfect_clear_double, "Perfect Clear (Double)", true, false);
		this->message_system.addMessage(*S::TETRIS_PERFECT_CLEAR);
		this->message_system.addMessage(*S::TETRIS_DOUBLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::PERFECT_CLEAR_TRIPLE>(int level)
	{
		this->addPoints(level, this->perfect_clear_triple, "Perfect Clear (Triple)", true, false);
		this->message_system.addMessage(*S::TETRIS_PERFECT_CLEAR);
		this->message_system.addMessage(*S::TETRIS_TRIPLE);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::PERFECT_CLEAR_TETRIS>(int level)
	{
		this->addPoints(level, this->perfect_clear_tetris, "Perfect Clear (Tetris)", true, false);
		this->message_system.addMessage(*S::TETRIS_PERFECT_CLEAR);
		this->message_system.addMessage(*S::TETRIS_TETRIS);
	}

	template<>
	void ScoreSystem::addPoints<ScoreSystem::ScoreReason::PERFECT_CLEAR_BACK_TO_BACK_TETRIS>(int level)
	{
		this->addPoints(level, this->perfect_clear_back_to_back_tetris, "Perfect Clear (Back to Back Tetris)", true, false);
		this->message_system.addMessage(*S::TETRIS_BACK_TO_BACK);
		this->message_system.addMessage(*S::TETRIS_PERFECT_CLEAR);
		this->message_system.addMessage(*S::TETRIS_TETRIS);
	}
}
