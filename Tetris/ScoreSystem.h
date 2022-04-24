#pragma once
#include <string>
#include <iostream>
#include "MessageSystem.h"
#include "FontRenderer.h"

namespace Tetris
{
	class ScoreSystem
	{
		using S = FontRenderer::SpecialSymbol;
	private:
		int points = 0;
		int combo = -1;
		int total_lines_cleared = 0;
		int total_tetrises = 0;

		const int points_single = 100;
		const int points_double = 300;
		const int points_triple = 500;
		const int points_tetris = 800;
		const int points_t_spin_mini = 100;
		const int points_t_spin = 400;
		const int points_t_spin_mini_single = 200;
		const int points_t_spin_single = 800;
		const int points_t_spin_mini_double = 1200;
		const int points_t_spin_double = 1200;
		const int points_t_spin_triple = 1600;
		const float back_to_back_multiplier = 1.5;
		const int combo_multiplier = 50;
		const int perfect_clear_single = 800;
		const int perfect_clear_double = 1200;
		const int perfect_clear_triple = 1800;
		const int perfect_clear_tetris = 2000;
		const int perfect_clear_back_to_back_tetris = 3200;
		const int points_soft_drop_per_cell = 1;
		const int points_hard_drop_per_cell = 2;

		void addPoints(int level, int amount, std::string reason, bool multiply_by_level = true, bool back_to_back = false, bool print = true);

	public:
		MessageSystem& message_system;
		ScoreSystem(MessageSystem& message_system);

		enum class ScoreReason
		{
			CHEATING,
			SINGLE,
			DOUBLE,
			TRIPLE,
			TETRIS,
			T_SPIN_MINI,
			T_SPIN,
			T_SPIN_MINI_SINGLE,
			T_SPIN_SINGLE,
			T_SPIN_MINI_DOUBLE,
			T_SPIN_DOUBLE,
			T_SPIN_TRIPLE,
			COMBO,
			SOFT_DROP,
			HARD_DROP,
			PERFECT_CLEAR_SINGLE,
			PERFECT_CLEAR_DOUBLE,
			PERFECT_CLEAR_TRIPLE,
			PERFECT_CLEAR_TETRIS,
			PERFECT_CLEAR_BACK_TO_BACK_TETRIS,
		};

		int getCombo();
		void resetCombo();
		void increaseCombo();

		int getTotalLinesCleared();
		void resetTotalLinesCleared();
		void increaseTotalLinesCleared();

		int getTotalTetrises();
		void resetTotalTetrises();
		void increaseTotalTetrises();

		float getTetrisRate();

		int getPoints();
		void resetPoints();
		template<ScoreReason reason> void addPoints();
		template<ScoreReason reason> void addPoints(int level);
		template<ScoreReason reason> void addPoints(int level, bool back_to_back);
	};
}
