#pragma once
#include "Playfield.h"
#include "Tetrimino.h"
#include "ShaderProgram.h"
#include "RandomBag.h"
#include "ScoreSystem.h"
#include <vector>
#include <memory>
#include <chrono>
#include "MathHelper.h"
#include <deque>
#include <mutex>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <irrKlang.h>
#include "FontRenderer.h"
#include "MessageSystem.h"
#include <functional>

namespace Tetris
{
	using MathHelper::Point;
	using RNG = RandomGenerator;
	using Letter = Tetrimino::TetriminoLetter;
	using Rotation = Tetrimino::RotationState;
	using clock = std::chrono::high_resolution_clock;
	using fsec = std::chrono::duration<float>;
	using ScoreReason = ScoreSystem::ScoreReason;

	struct Keybinds
	{
		std::vector<int> rotate_cw;
		std::vector<int> rotate_ccw;
		std::vector<int> move_left;
		std::vector<int> move_right;
		std::vector<int> hard_drop;
		std::vector<int> hold;
		std::vector<int> soft_drop;
		std::vector<int> keybinds_change;
		std::vector<int> pause;
		std::vector<int> start;
		std::vector<int> mute_music;
	};

	enum class TetrisSound
	{
		SOUND_COMBO,
		SOUND_HARD_DROP,
		SOUND_HOLD,
		SOUND_KOROBEINIKI,
		SOUND_LEVEL_UP,
		SOUND_LINE_CLEAR,
		SOUND_LINE_CLEAR_2,
		SOUND_LOCK,
		SOUND_MOVE,
		SOUND_ROTATE,
		SOUND_T_SPIN_MOVE,
		SOUND_TOP_OUT
	};

	class Tetris
	{
	private:
		const int window_width;
		const int window_height;
		const int window_width_half;
		const int window_height_half;
		irrklang::ISound* korobeiniki = nullptr;
		irrklang::ISoundEngine* sound_engine = nullptr;
		std::shared_ptr<FontRenderer> font_renderer;
		std::shared_ptr<Playfield> playfield;
		std::map<Point, std::shared_ptr<Monomino>> monominoes_on_playfield;
		std::unique_ptr<Tetrimino> current_piece;
		std::unique_ptr<Tetrimino> current_ghost_piece;
		std::unique_ptr<Tetrimino> held_piece_preview;
		Geometry::Rectangle pause_overlay;
		Geometry::Rectangle death_overlay;
		Geometry::Rectangle start_overlay;
		int current_piece_frames_lived = 0;
		clock::time_point last_processed;
		const double target_process_rate = 60.0;	//60 logic "frames"
		const double target_process_hz = 1.0 / this->target_process_rate;	//soft-hardcode so it's not recalculated every loop iteration
		bool logic_started = false;
		double gravity = 1.0 / 60.0; //G unit - cells per logic process
		double gravity_counter = 0.0;
		int level = 1;
		bool stop_signal = false;
		bool keybinds_alternative = false;
		Keybinds* keybinds;
		Keybinds keybinds_a;
		Keybinds keybinds_b;
		std::map<int, bool> key_states;	//[] on non-existent key will return false by default
		int last_direction_key = -1;	//0=left or 1=right, for better gameplay purposes and because this->key_states doesn't care about order
		int last_rotation_key = -1;	//0=rotate_cw or 1=rotate_ccw
		int lock_delay = 30;	//in frames
		int lock_delay_counter = 0;
		const int lock_move_amount = 15;	//set to 0 for infinity mode (can stall indefinitely by just rotating and moving the piece)
		int lock_move_counter = 0;
		bool piece_touched_ground = false;
		const int auto_repeat_delay = 10;	//frames before auto-repeat of left/right key happens (roughly 170ms = 10.2frames)
		const int auto_repeat_speed = 3;	//frames between each auto-repeat iteration	(50ms = 3 frames)
		int auto_repeat_counter = 0;
		Letter hold_piece = Letter::TETRIMINO_NONE;
		bool hold_ready = true;
		RandomBag<Letter> next_pieces_bag;
		std::deque<Letter> next_pieces;
		const int next_preview_amount = 3;
		bool lines_are_being_cleared = false;
		int clear_lines_counter = 0;
		int clear_lines_magnitude = 0;
		int clear_lines_current_x = 0;
		int clear_lines_bottom_y = 0;
		std::vector<int> lines_to_clear;
		ScoreSystem score_system;
		bool t_spin = false;
		bool mini_t_spin = false;
		bool check_empty_t_spin = false;
		bool check_combo = false;
		bool back_to_back_tetris = false;
		bool back_to_back_t_spin = false;
		bool pause = false;
		std::string sounds_path = "";
		MessageSystem message_system;
		bool death = false;
		bool game_started = false;
		int countdown_counter = static_cast<int>(3 * this->target_process_rate);	//3 seconds
		bool mute_music = false;
		const float music_volume = 0.5f;

		void removeCurrentPiece();
		void lockCurrentPiece(bool gray_out = false);
		void nextCurrentPiece(bool ready_hold = false, Letter letter = Letter::TETRIMINO_NONE);
		void addMonomino(std::shared_ptr<Monomino> monomino);
		void addMonominoes(std::vector<std::shared_ptr<Monomino>> monominoes, bool locked = false);
		void clearPlayfield();
		void topOut();
		bool isAnyMonominoAt(int x, int y);
		bool checkCollisionAt(std::unique_ptr<Tetrimino>& piece);
		bool checkCollisionAt(std::unique_ptr<Tetrimino>& piece, int center_x, int center_y);
		bool isOutOfPlayfield(std::unique_ptr<Tetrimino>& piece);
		bool isOutOfPlayfield(std::unique_ptr<Tetrimino>& piece, int center_x, int center_y);
		bool isOutOfPlayfield(int target_x, int target_y);
		double getCurrentGravity();
		double getGravity(int level);
		int getLockDelay(int level);
		glm::ivec2 getMoveDirectionVector();
		glm::ivec2 getRotationDirectionVector();
		int movePieceFor(std::unique_ptr<Tetrimino>& piece, int x, int y);	//0 - success, 1 - nullptr, 2 - collision, 3 - out of playfield
		bool processHold(bool not_first_frame, bool has_hold_input);
		void processRotation(bool not_first_frame);
		void processShift(bool not_first_frame);
		void processGhost();
		void processGravity(bool not_first_frame, bool hard_drop, bool soft_drop);
		void processClearLine();
		void refillBag();
		Letter getNextPieceLetter();
		void makePieceFromLetter(std::unique_ptr<Tetrimino>& piece, Letter letter);
		void restart();
		void playSound(TetrisSound sound);
		void startCountdown(int duration);
		void initKeyMappings();
		void addKeyMapping(Keybinds& keybinds, std::vector<int> Keybinds::* field, int key);
		bool isPressed(std::vector<int> Keybinds::* field);
		void unpress(std::vector<int> Keybinds::* field);

	public:
		Tetris(int screen_width, int screen_height, irrklang::ISoundEngine* sound_engine, std::string sounds_path, std::shared_ptr<FontRenderer> font_renderer);
		float getCellSize();
		float getBoardTopBoundary();
		void draw(std::shared_ptr<ShaderProgram> shader_program);
		void process();
		bool hasGameStarted();
		void stop();
		bool hasStopped();
		void handleKeyPress(int key);
		void handleKeyRelease(int key);
		bool isPaused();
		bool isDead();
		int getScore();
		~Tetris();
	};
}
