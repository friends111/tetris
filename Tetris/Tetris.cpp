#include "Tetris.h"

namespace Tetris
{
	Tetris::Tetris(int screen_width, int screen_height, irrklang::ISoundEngine* sound_engine, std::string sounds_path, std::shared_ptr<FontRenderer> font_renderer)
		: score_system(this->message_system), window_width(screen_width), window_height(screen_height), window_width_half(screen_width / 2), window_height_half(screen_height / 2)
	{
		this->playfield = std::make_unique<Playfield>(screen_width, screen_height);
		this->sound_engine = sound_engine;
		this->sounds_path = sounds_path;
		this->korobeiniki = this->sound_engine->play2D((this->sounds_path + "korobeiniki.ogg").c_str(), true, false, true);
		if (this->korobeiniki != nullptr)
		{
			this->korobeiniki->setVolume(this->music_volume);
			this->korobeiniki->setIsPaused(true);
		}
		else
		{
			std::cerr << "[TETRIS]: Music file not found!" << std::endl;
		}

		this->keybinds = &this->keybinds_a;
		this->initKeyMappings();

		this->font_renderer = font_renderer;

		this->refillBag();
		Letter letter = this->getNextPieceLetter();
		this->makePieceFromLetter(this->current_piece, letter);
		this->makePieceFromLetter(this->current_ghost_piece, letter);
		this->current_ghost_piece->setAsGhost();

		this->pause_overlay.initFrom2D({ 0, 0 }, { this->window_width, this->window_height });
		this->pause_overlay.setColor(0, 0, 0, 0.5f);

		this->death_overlay.initFrom2D({ 0, 0 }, { this->window_width, this->window_height });
		this->death_overlay.setColor(0.1f, 0, 0, 0.8f);

		this->start_overlay.initFrom2D({ 0, 0 }, { this->window_width, this->window_height });
		this->start_overlay.setColor(0, 0.1f, 0, 0.5f);

		this->logic_started = true;
	}

	void Tetris::addMonomino(std::shared_ptr<Monomino> monomino)
	{
		this->monominoes_on_playfield[monomino->getFinalPos()] = monomino;
	}

	void Tetris::addMonominoes(std::vector<std::shared_ptr<Monomino>> monominoes, bool locked)
	{
		for (std::shared_ptr<Monomino>& m : monominoes)
		{
			if (locked)
			{
				m->setColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
			}
			this->addMonomino(m);
		}
	}

	void Tetris::clearPlayfield()
	{
		this->monominoes_on_playfield.clear();
		std::cout << "[TETRIS]: Playfield cleared." << std::endl;
	}

	//Top out
	void Tetris::topOut()
	{
		std::cout << "[TETRIS]: Top out!" << std::endl;
		//this->lockCurrentPiece(true);
		this->death = true;
		if (this->korobeiniki != nullptr)
		{
			this->korobeiniki->setIsPaused(true);
		}
		this->playSound(TetrisSound::SOUND_TOP_OUT);
		//this->restart();
	}

	bool Tetris::isAnyMonominoAt(int x, int y)
	{
		return this->monominoes_on_playfield.find({ x, y }) != this->monominoes_on_playfield.end();
	}

	bool Tetris::checkCollisionAt(std::unique_ptr<Tetrimino>& piece, int center_x, int center_y)
	{
		std::vector<std::shared_ptr<Monomino>> monominoes = piece->getMonominoes();

		for (std::shared_ptr<Monomino>& m : monominoes)
		{
			glm::ivec2 pos_d = m->getFinalOffset();
			if (this->isAnyMonominoAt(center_x + pos_d.x, center_y + pos_d.y))
			{
				return true;
			}
		}

		return false;
	}

	bool Tetris::checkCollisionAt(std::unique_ptr<Tetrimino>& piece)
	{
		glm::ivec2 pos = piece->getPos();
		return this->checkCollisionAt(piece, pos.x, pos.y);
	}

	bool Tetris::isOutOfPlayfield(std::unique_ptr<Tetrimino>& piece)
	{
		glm::ivec2 pos = piece->getPos();
		return this->isOutOfPlayfield(piece, pos.x, pos.y);
	}

	bool Tetris::isOutOfPlayfield(std::unique_ptr<Tetrimino>& piece, int center_x, int center_y)
	{
		std::vector<std::shared_ptr<Monomino>> monominoes = piece->getMonominoes();

		for (std::shared_ptr<Monomino>& m : monominoes)
		{
			glm::ivec2 pos_d = m->getFinalOffset();
			if (this->isOutOfPlayfield(center_x + pos_d.x, center_y + pos_d.y))
			{
				return true;
			}
		}

		return false;
	}

	bool Tetris::isOutOfPlayfield(int target_x, int target_y)
	{
		glm::ivec2 playfield_size = this->playfield->getSize();

		if (target_x < 0 || target_x >= playfield_size.x)
		{
			return true;
		}

		if (target_y < 0 || target_y >= playfield_size.y)
		{
			return true;
		}
	
		return false;
	}

	double Tetris::getCurrentGravity()
	{
		return this->gravity;
	}

	double Tetris::getGravity(int level)
	{
		level = std::min(20, level);
		double time = std::pow((0.8 - ((static_cast<double>(level) - 1.0) * 0.007)), static_cast<double>(level) - 1.0);
		return 1.0 / (time * this->target_process_rate);
		//return this->gravity + (static_cast<double>(level) - 1.0)/15.0;
	}

	int Tetris::getLockDelay(int level)
	{
		if (level <= 20)
		{
			return 30;
		}

		level = std::min(30, level);	//clamp to 21-30

		return 68 - 2 * level;	//arbitrary formula because i can't find any...
		//starting from 21 level: 26, 24, 22, 20, 18, 16, 14, 12, 10, and finally 8 at level 30 and higher
	}

	glm::ivec2 Tetris::getMoveDirectionVector()
	{
		bool move_left = this->isPressed(&Keybinds::move_left);
		bool move_right = this->isPressed(&Keybinds::move_right);

		if (move_left)
		{
			if (move_right && this->last_direction_key == 1)
			{
				return glm::ivec2(1, 0);
			}
			else
			{
				return glm::ivec2(-1, 0);
			}
		}
		else if (move_right)
		{
			if (move_left && this->last_direction_key == 0)
			{
				return glm::ivec2(-1, 0);
			}
			else
			{
				return glm::ivec2(1, 0);
			}
		}

		return glm::ivec2(0, 0);
	}

	glm::ivec2 Tetris::getRotationDirectionVector()
	{
		bool rotate_cw = this->isPressed(&Keybinds::rotate_cw);
		bool rotate_ccw = this->isPressed(&Keybinds::rotate_ccw);

		if (rotate_cw)
		{
			if (rotate_ccw && this->last_rotation_key == 1)
			{
				return glm::ivec2(0, -1);
			}
			else
			{
				return glm::ivec2(0, 1);
			}
		}
		else if (rotate_ccw)
		{
			if (rotate_cw && this->last_rotation_key == 0)
			{
				return glm::ivec2(0, 1);
			}
			else
			{
				return glm::ivec2(0, -1);
			}
		}

		return glm::ivec2(0, 0);
	}

	int Tetris::movePieceFor(std::unique_ptr<Tetrimino>& piece, int x, int y)
	{
		if (piece == nullptr)
		{
			return 1;
		}

		if (x == 0 && y == 0)
		{
			return 0;
		}

		glm::ivec2 current_pos = piece->getPos();

		glm::ivec2 dir = glm::ivec2(x, y);
		glm::ivec2 pos_new = current_pos + dir;

		bool new_collision = this->checkCollisionAt(piece, pos_new.x, pos_new.y);
		bool new_out_of_playfield = this->isOutOfPlayfield(piece, pos_new.x, pos_new.y);

		if (new_collision)
		{
			return 2;
		}

		if (new_out_of_playfield)
		{
			return 3;
		}

		this->lock_delay_counter = 0;
		piece->moveFor(dir.x, dir.y);

		return 0;
	}

	void Tetris::removeCurrentPiece()
	{
		//std::cout << "[TETRIS]: Removing current piece." << std::endl;
		this->current_piece.reset();
		this->current_ghost_piece.reset();
	}

	void Tetris::lockCurrentPiece(bool gray_out)
	{
		this->check_empty_t_spin = true;
		this->addMonominoes(this->current_piece->getMonominoes(), gray_out);
		this->removeCurrentPiece();
	}

	void Tetris::nextCurrentPiece(bool ready_hold, Letter letter)
	{
		if (ready_hold)
		{
			this->hold_ready = true;
		}

		this->current_piece_frames_lived = 0;

		if (letter == Letter::TETRIMINO_NONE)
		{
			//letter = static_cast<Letter>(RandomGenerator::getRandomInt(1, 7));
			letter = this->getNextPieceLetter();
		}

		this->removeCurrentPiece();
		this->makePieceFromLetter(this->current_piece, letter);
		this->makePieceFromLetter(this->current_ghost_piece, letter);
		this->current_ghost_piece->setAsGhost();

		this->lock_delay_counter = 0;
		this->lock_move_counter = 0;
		this->piece_touched_ground = false;

		glm::ivec2 pos = this->current_piece->getPos();
		if (this->checkCollisionAt(this->current_piece, pos.x, pos.y))
		{
			if (this->checkCollisionAt(this->current_piece, pos.x, pos.y - 1))
			{
				this->current_piece->moveFor(0, -1);
				this->lockCurrentPiece(true);
				this->topOut();
				return;
			}

			this->current_piece->moveFor(0, -1);
		}
	}

	float Tetris::getCellSize()
	{
		return this->playfield->getCellSize();
	}

	float Tetris::getBoardTopBoundary()
	{
		return this->playfield->getBoardTopBoundary();
	}

	void Tetris::draw(std::shared_ptr<ShaderProgram> shader_program)
	{
		//std::cout << "[TETRIS]: Starting drawing..." << std::endl;
		//mtx.lock();
		this->playfield->draw(shader_program);
		//mtx.unlock();

		//mtx.lock();
		using S = FontRenderer::SpecialSymbol;
		std::string points = std::to_string(this->score_system.getPoints());
		std::string level = std::to_string(this->level);
		glm::vec2 pos = this->playfield->gridToScreenCoordinates(-6, 30);
		std::string lines = std::to_string(this->score_system.getTotalLinesCleared());
		this->font_renderer->renderText(shader_program, "SCORE", { pos.x , pos.y + 0 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		this->font_renderer->renderText(shader_program, points, { pos.x, pos.y + 35 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		this->font_renderer->renderText(shader_program, "LEVEL", { pos.x, pos.y + 100 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		this->font_renderer->renderText(shader_program, level, { pos.x, pos.y + 135 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		this->font_renderer->renderText(shader_program, "LINES", { pos.x, pos.y + 200 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		this->font_renderer->renderText(shader_program, lines, { pos.x, pos.y + 235 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);

		glm::vec2 pos_hold = this->playfield->gridToScreenCoordinates(-6, 21);
		this->font_renderer->renderText(shader_program, "HOLD", { pos_hold.x, pos_hold.y - 5 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);

		glm::vec2 pos_keybinds = this->playfield->gridToScreenCoordinates(15, 19);
		if (this->keybinds_alternative)
		{
			this->font_renderer->renderText(shader_program, "KEYBINDS: B", { pos_keybinds.x, pos_keybinds.y }, 2, { 1, 0, 0.8, 1 }, FontRenderer::TextAlignment::CENTER);
		}
		else
		{
			this->font_renderer->renderText(shader_program, "KEYBINDS: A", { pos_keybinds.x, pos_keybinds.y }, 2, { 0, 1, 0.2, 1 }, FontRenderer::TextAlignment::CENTER);
		}

		if (this->korobeiniki != nullptr)
		{
			glm::vec2 pos_mute = this->playfield->gridToScreenCoordinates(18, 19);
			if (this->mute_music)
			{
				this->font_renderer->renderText(shader_program, *S::MUSIC_MUTED, { pos_mute.x, pos_mute.y }, 2, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			}
			else
			{
				this->font_renderer->renderText(shader_program, *S::MUSIC, { pos_mute.x, pos_mute.y }, 2, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			}
		}

		if (this->score_system.getTotalLinesCleared() > 0)
		{
			glm::vec2 pos_info = this->playfield->gridToScreenCoordinates(-6, 40);
			std::string tetris_rate = std::to_string(static_cast<int>(this->score_system.getTetrisRate()));
			this->font_renderer->renderText(shader_program, *S::TETRIS_TETRIS + " RATE: " + tetris_rate + "%%", { pos_info.x, pos_info.y - 5 }, 2, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		}

		int combo = this->score_system.getCombo();
		if (combo > 0)
		{
			glm::vec2 pos_combo = this->playfield->gridToScreenCoordinates(-6, 18);
			std::string combo_str = std::to_string(combo);
			this->font_renderer->renderText(shader_program, *S::TETRIS_COMBO + ": " + combo_str, { pos_combo.x, pos_combo.y + 15 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
		}

		int msg_count = 3;
		std::list<MessageSystem::Message> messages = this->message_system.getMessages();
		for (MessageSystem::Message msg : messages)
		{
			if (msg_count <= 0)
			{
				break;
			}

			this->font_renderer->renderText(shader_program, msg.text, { pos.x, pos.y - 145 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::TOP_CENTER);
			pos.y += 35;
			msg_count--;
		}
		//mtx.unlock();

		glm::vec2 screen_center = { this->window_width_half, this->window_height_half };

		if (!this->game_started)
		{
			this->start_overlay.draw(shader_program);
			this->font_renderer->renderText(shader_program, "PRESS ENTER TO START.", screen_center, 6, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			
			if (this->keybinds_alternative)
			{
				this->font_renderer->renderText(shader_program, "CURRENT CONTROLS (ALTERNATIVE):", { screen_center.x - 220, screen_center.y + 50 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "LEFT/RIGHT ARROW KEYS - MOVE LEFT/RIGHT", { screen_center.x - 220, screen_center.y + 80 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "UP/DOWN ARROW KEYS - ROTATE CLOCKWISE/COUNTER CLOCKWISE", { screen_center.x - 220, screen_center.y + 110 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "C - HOLD", { screen_center.x - 220, screen_center.y + 140 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "V - SOFT DROP", { screen_center.x - 220, screen_center.y + 170 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "SPACE - HARD DROP", { screen_center.x - 220, screen_center.y + 200 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "ESC - PAUSE", { screen_center.x - 220, screen_center.y + 230 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "A - CHANGE KEYBINDS", { screen_center.x - 220, screen_center.y + 260 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "M - MUTE/UNMUTE MUSIC", { screen_center.x - 220, screen_center.y + 290 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
			}
			else
			{
				this->font_renderer->renderText(shader_program, "CURRENT CONTROLS (GUIDELINE):", { screen_center.x - 220, screen_center.y + 50 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "LEFT/RIGHT ARROW KEYS [KEY PAD 4/6] - MOVE LEFT/RIGHT", { screen_center.x - 220, screen_center.y + 80 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "X/UP ARROW KEY [KP 1/5/9] - ROTATE CLOCKWISE", { screen_center.x - 220, screen_center.y + 110 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "Z/LCTRL ARROW KEY [KP 3/7] - ROTATE COUNTER CLOCKWISE", { screen_center.x - 220, screen_center.y + 140 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "C/LSHIFT [KP 0] - HOLD", { screen_center.x - 220, screen_center.y + 170 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "DOWN ARROW KEY [KP 2] - SOFT DROP", { screen_center.x - 220, screen_center.y + 200 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "SPACE [KP 8] - HARD DROP", { screen_center.x - 220, screen_center.y + 230 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "ESC/F1 - PAUSE", { screen_center.x - 220, screen_center.y + 260 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "A - CHANGE KEYBINDS", { screen_center.x - 220, screen_center.y + 290 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
				this->font_renderer->renderText(shader_program, "M - MUTE/UNMUTE MUSIC", { screen_center.x - 220, screen_center.y + 320 }, 2, { 0.5, 0.5, 0.5, 1 }, FontRenderer::TextAlignment::CENTER_LEFT);
			}

			return;
		}

		if (this->countdown_counter != 0)
		{
			int seconds = static_cast<int>(std::ceil(this->countdown_counter / this->target_process_rate));
			this->font_renderer->renderText(shader_program, std::to_string(seconds), screen_center, 16, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			return;
		}

		if (this->pause)
		{
			this->pause_overlay.draw(shader_program);
			this->font_renderer->renderText(shader_program, "[PAUSED]", screen_center, 11, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			return;
		}

		if (this->game_started)
		{
			shader_program->setUniformValue("cut_off", 1);
			shader_program->setUniformValue("is_block", 1);

			//mtx.lock();
			for (auto& pair : this->monominoes_on_playfield)
			{
				std::shared_ptr<Monomino>& m = pair.second;
				if (m != nullptr)
				{
					bool inverse = false;
					if (this->lines_are_being_cleared)
					{
						for (auto& y : this->lines_to_clear)
						{
							if (m->getFinalPos().y == y)
							{
								inverse = true;
								shader_program->setUniformValue("inverse", 1);
								break;
							}
						}
					}
					m->draw(shader_program, this->playfield);
					if (inverse)
					{
						shader_program->setUniformValue("inverse", 0);
					}
				}
			}
			//mtx.unlock();

			//mtx.lock();
			if (this->current_ghost_piece != nullptr)
			{
				shader_program->setUniformValue("is_ghost", 1);
				for (std::shared_ptr<Monomino>& m : this->current_ghost_piece->getMonominoes())
				{
					if (m != nullptr)
					{
						m->draw(shader_program, this->playfield);
					}
				}
				shader_program->setUniformValue("is_ghost", 0);
			}
			//mtx.unlock();

			//mtx.lock();
			if (this->current_piece != nullptr)
			{
				for (std::shared_ptr<Monomino>& m : this->current_piece->getMonominoes())
				{
					if (m != nullptr)
					{
						m->draw(shader_program, this->playfield);
					}
				}
			}
			//mtx.unlock();

			shader_program->setUniformValue("cut_off", 0);

			//mtx.lock();
			//TODO: rewrite this shit
			int i = 0;
			float cell_size = this->playfield->getCellSize();
			for (Letter& letter : this->next_pieces)
			{
				if (i >= this->next_preview_amount)
				{
					break;
				}
				std::unique_ptr<Tetrimino> t;
				this->makePieceFromLetter(t, letter);
				t->moveTo(15, 21 + i * 3);
				i++;
				glm::vec2 preview_pos = t->getPreviewPos();
				glm::vec2 offset = glm::vec2(std::floor(cell_size * preview_pos.x), std::floor(cell_size * preview_pos.y));

				for (std::shared_ptr<Monomino>& m : t->getMonominoes())
				{
					if (m != nullptr)
					{
						glm::ivec2 grid_pos = m->getFinalPos();
						glm::vec2 screen_pos = this->playfield->gridToScreenCoordinates(grid_pos.x, grid_pos.y);
						m->draw(shader_program, this->playfield, screen_pos + offset);
					}
				}
			}
			//mtx.unlock();

			//mtx.lock();
			if (this->held_piece_preview != nullptr)
			{
				glm::vec2 preview_pos = this->held_piece_preview->getPreviewPos();
				glm::vec2 offset = glm::vec2(std::floor(cell_size * preview_pos.x), std::floor(cell_size * preview_pos.y));
				for (std::shared_ptr<Monomino>& m : this->held_piece_preview->getMonominoes())
				{
					if (m != nullptr)
					{
						glm::ivec2 grid_pos = m->getFinalPos();
						glm::vec2 screen_pos = this->playfield->gridToScreenCoordinates(grid_pos.x, grid_pos.y);
						m->draw(shader_program, this->playfield, screen_pos + offset);
					}
				}
			}
			//mtx.unlock();

			shader_program->setUniformValue("is_block", 0);
		}

		if (this->death)
		{
			this->death_overlay.draw(shader_program);
			this->font_renderer->renderText(shader_program, "GAME OVER!", { screen_center.x, screen_center.y - 75 }, 11, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			this->font_renderer->renderText(shader_program, "FINAL SCORE: " + points, { screen_center.x, screen_center.y + 10 }, 6, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			this->font_renderer->renderText(shader_program, "PRESS ENTER TO START A NEW GAME.", { screen_center.x, screen_center.y + 75 }, 4, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::CENTER);
			return;
		}

		//std::cout << "[TETRIS]: Finished drawing." << std::endl;
	}

	void Tetris::process()
	{
		if (!this->logic_started)
		{
			return;
		}

		clock::time_point now = clock::now();
		fsec diff = now - this->last_processed;

		if (diff.count() < this->target_process_hz)
		{
			return;
		}

		//std::cout << "Last frame took " << diff.count() << "s" << std::endl;

		this->last_processed = now;

		glfwPollEvents();

		if (!this->game_started)
		{
			return;
		}

		if (this->countdown_counter != 0)
		{
			this->countdown_counter--;
			if (this->countdown_counter == 0)
			{
				if (this->korobeiniki != nullptr)
				{
					this->korobeiniki->setIsPaused(false);
				}
			}
			return;
		}

		if (this->death)
		{
			return;
		}

		if (this->pause)
		{
			return;
		}

		this->processClearLine();
		if (this->lines_are_being_cleared)
		{
			return;	// keep note of this return and change to condition if needed
		}

		bool has_hold_input = this->isPressed(&Keybinds::hold);
		this->unpress(&Keybinds::hold);	//only handle piece holding once per C press

		bool hard_drop = this->isPressed(&Keybinds::hard_drop);
		this->unpress(&Keybinds::hard_drop);	//only handle hard-drop once per Spacebar press

		bool soft_drop = this->isPressed(&Keybinds::soft_drop);

		//mtx.lock();
		if (this->current_piece == nullptr)
		{
			this->gravity_counter = 0.0;
			this->nextCurrentPiece(true);

			if (this->death)
			{
				return;
			}
		}
		else
		{
			this->gravity_counter += (soft_drop ? 20.0 : 1.0) * this->getCurrentGravity();
		}

		bool not_first_frame = this->current_piece_frames_lived >= 1;

		if (this->current_piece != nullptr)
		{
			//Hold
			bool was_piece_held = this->processHold(not_first_frame, has_hold_input);

			if (!was_piece_held)
			{
				//Rotation
				this->processRotation(not_first_frame);

				//Shift (horizontal movement)
				this->processShift(not_first_frame);

				//Ghost piece
				this->processGhost();

				//Gravity
				this->processGravity(not_first_frame, hard_drop, soft_drop);
			}

			//Other
			this->current_piece_frames_lived++;
			this->lock_delay_counter++;
			this->auto_repeat_counter++;
		}

		this->message_system.process();

		//mtx.unlock();
	}

	//Hold
	bool Tetris::processHold(bool not_first_frame, bool has_hold_input)
	{
		if (!not_first_frame)
		{
			return false;
		}

		if (!has_hold_input)
		{
			return false;
		}

		if (!this->hold_ready)
		{
			//std::cout << "[TETRIS]: Could not hold - already done this round!" << std::endl;
			return false;
		}

		Letter old_hold_piece = this->current_piece->getLetter();
		this->held_piece_preview.reset();
		//this->held_piece_preview = std::make_unique<Tetrimino>(Tetrimino(*this->current_piece));
		this->makePieceFromLetter(this->held_piece_preview, old_hold_piece);
		this->held_piece_preview->moveTo(-7, 23);
		this->removeCurrentPiece();
		this->nextCurrentPiece(false, this->hold_piece);
		this->hold_piece = old_hold_piece;
		this->hold_ready = false;
		this->playSound(TetrisSound::SOUND_HOLD);

		return true;
	}

	//Rotation
	void Tetris::processRotation(bool not_first_frame)
	{
		if (!not_first_frame)
		{
			return;
		}

		glm::ivec2 dir = this->getRotationDirectionVector();
		this->unpress(&Keybinds::rotate_cw);
		this->unpress(&Keybinds::rotate_ccw);

		if (dir.y == 0)
		{
			return;
		}

		this->lock_delay_counter = 0;

		bool clockwiseness = dir.y > 0;
		glm::ivec2 pos = this->current_piece->getPos();

		this->current_piece->rotate(clockwiseness);
		this->current_ghost_piece->rotate(clockwiseness);

		int kick_counter = 0;
		bool success = false;
		for (int i = 0; i < 5; i++)
		{
			this->current_piece->kick(i, clockwiseness);
			this->current_ghost_piece->kick(i, clockwiseness);

			if (!this->checkCollisionAt(this->current_piece) && !this->isOutOfPlayfield(this->current_piece))
			{
				success = true;
				this->current_piece->kickToPos();
				this->current_ghost_piece->kickToPos();
				break;
			}

			kick_counter++;
		}

		if (success)
		{
			this->playSound(TetrisSound::SOUND_ROTATE);
		}
		else
		{
			this->current_piece->resetKick();
			this->current_ghost_piece->resetKick();
			this->current_piece->moveTo(pos.x, pos.y);
			this->current_ghost_piece->moveTo(pos.x, pos.y);
			this->current_piece->rotate(!clockwiseness);
			this->current_ghost_piece->rotate(!clockwiseness);
		}

		this->lock_move_counter++;
		this->t_spin = false;
		this->mini_t_spin = false;
		//potential T-Spin check
		if (success && this->current_piece->getLetter() == Letter::TETRIMINO_T)
		{
			glm::ivec2 pos = this->current_piece->getMonominoes()[0]->getFinalPos();
			int corner_count = 0;
			int front_corner_count = 0;
			Rotation rot = this->current_piece->getRotationState();

			if (this->isAnyMonominoAt(pos.x - 1, pos.y - 1) || this->isOutOfPlayfield(pos.x - 1, pos.y - 1))	//top left
			{
				if (rot == Rotation::ROTATION_NONE || rot == Rotation::ROTATION_ONCE_COUNTER_CLOCKWISE)
				{
					front_corner_count++;
				}
				corner_count++;
			}
			if (this->isAnyMonominoAt(pos.x + 1, pos.y - 1) || this->isOutOfPlayfield(pos.x + 1,  pos.y - 1))	//top right
			{
				if (rot == Rotation::ROTATION_NONE || rot == Rotation::ROTATION_ONCE_CLOCKWISE)
				{
					front_corner_count++;
				}
				corner_count++;
			}
			if (this->isAnyMonominoAt(pos.x - 1, pos.y + 1) || this->isOutOfPlayfield(pos.x - 1, pos.y + 1))	//bottom left
			{
				if (rot == Rotation::ROTATION_DOUBLE|| rot == Rotation::ROTATION_ONCE_COUNTER_CLOCKWISE)
				{
					front_corner_count++;
				}
				corner_count++;
			}
			if (this->isAnyMonominoAt(pos.x + 1, pos.y + 1) || this->isOutOfPlayfield(pos.x + 1, pos.y + 1))	//bottom right
			{
				if (rot == Rotation::ROTATION_DOUBLE || rot == Rotation::ROTATION_ONCE_CLOCKWISE)
				{
					front_corner_count++;
				}
				corner_count++;
			}

			if (corner_count >= 3)
			{
				if (kick_counter == 5 || front_corner_count == 2)
				{
					this->t_spin = true;
					this->playSound(TetrisSound::SOUND_T_SPIN_MOVE);
					std::cout << "[TETRIS]: T-Spin move!" << std::endl;
				}
				else
				{
					this->t_spin = true;
					this->mini_t_spin = true;
					this->playSound(TetrisSound::SOUND_T_SPIN_MOVE);
					std::cout << "[TETRIS]: T-Spin Mini move!" << std::endl;
				}
			}
		}
	}

	//Shift (horizontal movement)
	void Tetris::processShift(bool not_first_frame)
	{
		if (!not_first_frame)
		{
			return;
		}

		glm::ivec2 dir = this->getMoveDirectionVector();

		if (dir.x == 0)
		{
			return;
		}

		bool before_auto_repeat = this->auto_repeat_counter == 0;
		bool auto_repeat_started = this->auto_repeat_counter >= this->auto_repeat_delay;
		bool auto_repeat_this_frame = (this->auto_repeat_counter - this->auto_repeat_delay) % this->auto_repeat_speed == 0;
		if (before_auto_repeat || (auto_repeat_started && auto_repeat_this_frame))
		{
			if (this->movePieceFor(this->current_piece, dir.x, 0) == 0)
			{
				this->playSound(TetrisSound::SOUND_MOVE);
				this->lock_move_counter++;
			}
		}
	}

	//Ghost
	void Tetris::processGhost()
	{
		glm::ivec2 ghost_pos = this->current_piece->getPos();
		while (true)
		{
			bool ghost_collision = this->checkCollisionAt(this->current_ghost_piece, ghost_pos.x, ghost_pos.y + 1);
			bool ghost_out_of_playfield = this->isOutOfPlayfield(this->current_ghost_piece, ghost_pos.x, ghost_pos.y + 1);

			if (ghost_collision || ghost_out_of_playfield)
			{
				break;
			}

			ghost_pos.y++;
		}

		this->current_ghost_piece->moveTo(ghost_pos.x, ghost_pos.y);
	}

	//Gravity
	void Tetris::processGravity(bool not_first_frame, bool hard_drop, bool soft_drop)
	{
		if (!not_first_frame)
		{
			return;
		}

		glm::ivec2 new_pos = this->current_piece->getPos();
		while (hard_drop || MathHelper::round(this->gravity_counter, 7) >= 1.0)
		{
			this->gravity_counter -= 1.0;
			if (this->gravity_counter < 0.0)
			{
				this->gravity_counter = 0.0;
			}

			bool lock_delay = this->lock_delay_counter >= this->lock_delay;
			bool lock_move = this->lock_move_counter >= this->lock_move_amount;
			bool collision = this->checkCollisionAt(this->current_piece, new_pos.x, new_pos.y + 1);
			bool out_of_playfield = this->isOutOfPlayfield(this->current_piece, new_pos.x, new_pos.y + 1);

			if (!this->piece_touched_ground)
			{
				this->lock_delay_counter = 0;
				this->lock_move_counter = 0;
			}

			if (collision || out_of_playfield)
			{
				this->piece_touched_ground = true;

				if (hard_drop || lock_delay || lock_move)
				{
					this->playSound(TetrisSound::SOUND_LOCK);
					this->current_piece->moveTo(new_pos.x, new_pos.y);
					this->check_combo = true;
					this->lockCurrentPiece();
					return;
				}
			}
			else
			{
				if (hard_drop)
				{
					this->score_system.addPoints<ScoreReason::HARD_DROP>();
				}
				else if (soft_drop)
				{
					this->score_system.addPoints<ScoreReason::SOFT_DROP>();
				}
				new_pos.y++;
			}
		}

		this->current_piece->moveTo(new_pos.x, new_pos.y);
	}

	//TODO: clean this up, it's a mess
	void Tetris::processClearLine()
	{
		if (this->lines_are_being_cleared)
		{
			if (this->clear_lines_counter == 0)
			{
				if (this->clear_lines_magnitude == 4)
				{
					this->playSound(TetrisSound::SOUND_LINE_CLEAR_2);
				}
				else
				{
					this->playSound(TetrisSound::SOUND_LINE_CLEAR);
				}
			}

			if (this->clear_lines_current_x >= this->playfield->getSize().x)
			{
				std::map<Point, std::shared_ptr<Monomino>> monominoes_copy = this->monominoes_on_playfield;
				this->monominoes_on_playfield.clear();
				for (auto& m : monominoes_copy)
				{
					int x = m.second->getFinalPos().x;
					int y = m.second->getFinalPos().y;
					if (y < this->clear_lines_bottom_y)
					{
						int dy = 0;
						for (int& ly : this->lines_to_clear)
						{
							if (y < ly)
							{
								dy++;
							}
						}
						m.second->moveFor(0, dy);
						glm::ivec2 new_pos = glm::ivec2(x, y) + glm::ivec2(0, dy);
					}
					glm::ivec2 p = m.second->getFinalPos();
					this->monominoes_on_playfield[{p.x, p.y}] = m.second;
				}

				//Points
				bool perfect_clear = this->monominoes_on_playfield.size() == 0;

				if (this->t_spin)
				{
					if (this->mini_t_spin)
					{
						if (this->clear_lines_magnitude == 1)	//T-Spin Mini Single
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_MINI_SINGLE>(this->level, this->back_to_back_t_spin);
						}
						else if (this->clear_lines_magnitude == 2)	//T-Spin Mini Double
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_MINI_DOUBLE>(this->level, this->back_to_back_t_spin);
						}
					}
					else
					{
						if (this->clear_lines_magnitude == 1)	//T-Spin Single
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_SINGLE>(this->level, this->back_to_back_t_spin);
						}
						else if (this->clear_lines_magnitude == 2)	//T-Spin Double
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_DOUBLE>(this->level, this->back_to_back_t_spin);
						}
						else if (this->clear_lines_magnitude == 3)	//T-Spin Triple
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_TRIPLE>(this->level, this->back_to_back_t_spin);
						}
					}

					this->back_to_back_t_spin = true;
				}
				else
				{
					if (this->clear_lines_magnitude == 1)	//Single
					{
						this->back_to_back_tetris = false;
						this->back_to_back_t_spin = false;

						if (perfect_clear)
						{
							this->score_system.addPoints<ScoreReason::PERFECT_CLEAR_SINGLE>(this->level);
						}
						else
						{
							this->score_system.addPoints<ScoreReason::SINGLE>(this->level);
						}
					}
					else if (this->clear_lines_magnitude == 2)	//Double
					{
						this->back_to_back_tetris = false;
						this->back_to_back_t_spin = false;

						if (perfect_clear)
						{
							this->score_system.addPoints<ScoreReason::PERFECT_CLEAR_DOUBLE>(this->level);
						}
						else
						{
							this->score_system.addPoints<ScoreReason::DOUBLE>(this->level);
						}
					}
					else if (this->clear_lines_magnitude == 3)	//Triple
					{
						this->back_to_back_tetris = false;
						this->back_to_back_t_spin = false;

						if (perfect_clear)
						{
							this->score_system.addPoints<ScoreReason::PERFECT_CLEAR_TRIPLE>(this->level);
						}
						else
						{
							this->score_system.addPoints<ScoreReason::TRIPLE>(this->level);
						}
					}
					else if (this->clear_lines_magnitude == 4)	//Tetris
					{
						this->score_system.increaseTotalTetrises();

						if (this->back_to_back_tetris)
						{
							if (perfect_clear)
							{
								this->score_system.addPoints<ScoreReason::PERFECT_CLEAR_BACK_TO_BACK_TETRIS>(this->level);
							}
							else
							{
								this->score_system.addPoints<ScoreReason::TETRIS>(this->level, true);
							}
						}
						else
						{
							if (perfect_clear)
							{
								this->score_system.addPoints<ScoreReason::PERFECT_CLEAR_TETRIS>(this->level);
							}
							else
							{
								this->score_system.addPoints<ScoreReason::TETRIS>(this->level, false);
							}
						}

						this->back_to_back_tetris = true;
						this->back_to_back_t_spin = false;
					}
					else
					{
						this->back_to_back_tetris = false;
						this->back_to_back_t_spin = false;
						this->score_system.addPoints<ScoreReason::CHEATING>();
					}
				}

				this->score_system.increaseCombo();
				this->check_combo = false;

				if (this->score_system.getCombo() > 0)
				{
					this->playSound(TetrisSound::SOUND_COMBO);
					std::cout << "[TETRIS]: Combo: " << this->score_system.getCombo() << std::endl;
					this->score_system.addPoints<ScoreReason::COMBO>(this->level);
				}

				//level is increased after line clear
				for (int i = 0; i < this->clear_lines_magnitude; i++)
				{
					this->score_system.increaseTotalLinesCleared();
					if (this->score_system.getTotalLinesCleared() % 10 == 0)
					{
						this->level++;
						this->gravity = this->getGravity(this->level);
						this->lock_delay = this->getLockDelay(this->level);
						this->message_system.addMessage("Level up! (" + std::to_string(this->level) + ")");
						std::cout << "[TETRIS]: Level up! Current level: " << this->level << std::endl;
						this->playSound(TetrisSound::SOUND_LEVEL_UP);
					}
				}

				this->lines_are_being_cleared = false;
				this->clear_lines_counter = 0;
				this->clear_lines_magnitude = 0;
				this->lines_to_clear.clear();
				this->clear_lines_current_x = 0;
				this->t_spin = false;
				this->mini_t_spin = false;

				return;
			}

			if (this->clear_lines_counter % this->clear_lines_magnitude == 0)
			{
				for (int& y : this->lines_to_clear)
				{
					Point p = { this->clear_lines_current_x, y };
					if (this->monominoes_on_playfield.find(p) != this->monominoes_on_playfield.end())
					{
						this->monominoes_on_playfield[p].reset();
						this->monominoes_on_playfield.erase(p);
					}
				}

				this->clear_lines_current_x++;
			}

			this->clear_lines_counter++;
		}
		else
		{
			this->lines_are_being_cleared = false;

			int total_counter = 0;

			glm::ivec2 size = this->playfield->getSize();
			for (int y = 0; y < size.y; y++)
			{
				int counter = 0;

				for (int x = 0; x < size.x; x++)
				{
					if (this->isAnyMonominoAt(x, y))
					{
						total_counter++;
						counter++;
					}
				}

				if (counter == size.x)
				{
					this->clear_lines_bottom_y = y;
					this->clear_lines_magnitude++;
					this->lines_to_clear.push_back(y);
					this->lines_are_being_cleared = true;
				}
			}

			if (!this->lines_are_being_cleared)
			{
				if (this->check_empty_t_spin)
				{
					if (this->t_spin)
					{
						if (this->mini_t_spin)	//T-Spin Mini
						{
							this->score_system.addPoints<ScoreReason::T_SPIN_MINI>(this->level);
						}
						else	//T-Spin
						{
							this->score_system.addPoints<ScoreReason::T_SPIN>(this->level);
						}

						this->t_spin = false;
						this->mini_t_spin = false;
					}
				}

				if (this->check_combo)
				{
					if (this->score_system.getCombo() > 0)
					{
						std::cout << "[TETRIS]: Combo lost!" << std::endl;
					}
					this->check_combo = false;
					this->score_system.resetCombo();
				}
			}

			this->check_empty_t_spin = false;
		}
	}

	void Tetris::refillBag()
	{
		//move remaining pieces to the queue
		Letter piece = Letter::TETRIMINO_NONE;
		while ((piece = this->next_pieces_bag.takeOneRandomOut()) != Letter::TETRIMINO_NONE)
		{
			this->next_pieces.push_back(piece);
		}

		//empty and refill bag
		this->next_pieces_bag.empty();
		this->next_pieces_bag.insert(Letter::TETRIMINO_I);
		this->next_pieces_bag.insert(Letter::TETRIMINO_O);
		this->next_pieces_bag.insert(Letter::TETRIMINO_T);
		this->next_pieces_bag.insert(Letter::TETRIMINO_J);
		this->next_pieces_bag.insert(Letter::TETRIMINO_L);
		this->next_pieces_bag.insert(Letter::TETRIMINO_S);
		this->next_pieces_bag.insert(Letter::TETRIMINO_Z);
	}

	Tetrimino::TetriminoLetter Tetris::getNextPieceLetter()
	{
		if (this->next_pieces.size() <= 7)
		{
			this->refillBag();
		}

		Letter letter = this->next_pieces[0];
		this->next_pieces.pop_front();

		return letter;
	}

	void Tetris::makePieceFromLetter(std::unique_ptr<Tetrimino>& piece, Letter letter)
	{
		switch (letter)
		{
		case Letter::TETRIMINO_I:
			piece = std::make_unique<Tetrimino>(TetriminoI());
			break;
		case Letter::TETRIMINO_L:
			piece = std::make_unique<Tetrimino>(TetriminoL());
			break;
		case Letter::TETRIMINO_J:
			piece = std::make_unique<Tetrimino>(TetriminoJ());
			break;
		case Letter::TETRIMINO_S:
			piece = std::make_unique<Tetrimino>(TetriminoS());
			break;
		case Letter::TETRIMINO_Z:
			piece = std::make_unique<Tetrimino>(TetriminoZ());
			break;
		case Letter::TETRIMINO_O:
			piece = std::make_unique<Tetrimino>(TetriminoO());
			break;
		case Letter::TETRIMINO_T:
			piece = std::make_unique<Tetrimino>(TetriminoT());
			break;
		default:
			std::cerr << "[TETRIS]: Unknown Tetrimino type." << std::endl;
			return;
		}
	}

	void Tetris::restart()
	{
		std::cout << "[TETRIS]: Restarting game..." << std::endl;
		this->clearPlayfield();

		this->next_pieces.clear();
		this->refillBag();
		Letter letter = this->getNextPieceLetter();
		this->removeCurrentPiece();
		this->makePieceFromLetter(this->current_piece, letter);
		this->makePieceFromLetter(this->current_ghost_piece, letter);
		this->current_ghost_piece->setAsGhost();
		this->hold_piece = Letter::TETRIMINO_NONE;
		this->hold_ready = true;
		this->held_piece_preview.reset();
		this->level = 1;
		this->gravity = this->getGravity(this->level);
		this->gravity_counter = 0;
		this->current_piece_frames_lived = 0;
		this->lock_delay = this->getLockDelay(this->level);
		this->lock_delay_counter = 0;
		this->lock_move_counter = 0;
		this->piece_touched_ground = false;
		this->key_states.clear();
		this->last_direction_key = -1;
		this->auto_repeat_counter = 0;
		this->lines_are_being_cleared = false;
		this->clear_lines_counter = 0;
		this->clear_lines_magnitude = 0;
		this->lines_to_clear.clear();
		this->clear_lines_current_x = 0;
		this->score_system.resetCombo();
		this->score_system.resetPoints();
		this->back_to_back_tetris = false;
		this->back_to_back_t_spin = false;
		this->score_system.resetTotalLinesCleared();
		this->check_combo = false;
		this->score_system.resetTotalTetrises();
		this->countdown_counter = static_cast<int>(3 * this->target_process_rate);

		this->pause = false;
		this->death = false;
		std::cout << "[TETRIS]: New game started." << std::endl;
		if (this->korobeiniki != nullptr)
		{
			this->korobeiniki->setPlayPosition(0);
			this->korobeiniki->setIsPaused(true);
		}
	}

	void Tetris::playSound(TetrisSound sound)
	{
		std::string sound_file = "";

		switch (sound)
		{
		case TetrisSound::SOUND_HARD_DROP:
			sound_file = "hard_drop.wav";
			break;
		case TetrisSound::SOUND_HOLD:
			sound_file = "hold.wav";
			break;
		case TetrisSound::SOUND_LEVEL_UP:
			sound_file = "level_up.wav";
			break;
		case TetrisSound::SOUND_LINE_CLEAR:
			sound_file = "line_clear.wav";
			break;
		case TetrisSound::SOUND_LINE_CLEAR_2:
			sound_file = "line_clear2.wav";
			break;
		case TetrisSound::SOUND_LOCK:
			sound_file = "lock.wav";
			break;
		case TetrisSound::SOUND_MOVE:
			sound_file = "move.wav";
			break;
		case TetrisSound::SOUND_ROTATE:
			sound_file = "rotate.wav";
			break;
		case TetrisSound::SOUND_T_SPIN_MOVE:
			sound_file = "t_spin_move.wav";
			break;
		case TetrisSound::SOUND_TOP_OUT:
			sound_file = "top_out.wav";
			break;
		default:
			return;
		}

		this->sound_engine->play2D((this->sounds_path + sound_file).c_str());
	}

	void Tetris::startCountdown(int duration)
	{
		if (this->korobeiniki != nullptr)
		{
			this->korobeiniki->setIsPaused(true);
		}
		this->countdown_counter = duration;
	}

	bool Tetris::hasGameStarted()
	{
		return this->game_started;
	}

	void Tetris::stop()
	{
		this->logic_started = false;
		this->stop_signal = true;
		if (this->korobeiniki != nullptr)
		{
			this->korobeiniki->stop();
			this->korobeiniki->drop();
		}
	}

	bool Tetris::hasStopped()
	{
		return this->stop_signal;
	}

	void Tetris::initKeyMappings()
	{
		const static int key_up = glfwGetKeyScancode(GLFW_KEY_UP);
		const static int key_down = glfwGetKeyScancode(GLFW_KEY_DOWN);
		const static int key_left = glfwGetKeyScancode(GLFW_KEY_LEFT);
		const static int key_right = glfwGetKeyScancode(GLFW_KEY_RIGHT);
		const static int key_space = glfwGetKeyScancode(GLFW_KEY_SPACE);
		const static int key_a = glfwGetKeyScancode(GLFW_KEY_A);
		const static int key_c = glfwGetKeyScancode(GLFW_KEY_C);
		const static int key_m = glfwGetKeyScancode(GLFW_KEY_M);
		const static int key_v = glfwGetKeyScancode(GLFW_KEY_V);
		const static int key_x = glfwGetKeyScancode(GLFW_KEY_X);
		const static int key_z = glfwGetKeyScancode(GLFW_KEY_Z);
		const static int key_esc = glfwGetKeyScancode(GLFW_KEY_ESCAPE);
		const static int key_f1 = glfwGetKeyScancode(GLFW_KEY_F1);
		const static int key_enter = glfwGetKeyScancode(GLFW_KEY_ENTER);
		const static int key_lshift = glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT);
		const static int key_lctrl = glfwGetKeyScancode(GLFW_KEY_LEFT_CONTROL);
		const static int key_pad_0 = glfwGetKeyScancode(GLFW_KEY_KP_0);
		const static int key_pad_1 = glfwGetKeyScancode(GLFW_KEY_KP_1);
		const static int key_pad_2 = glfwGetKeyScancode(GLFW_KEY_KP_2);
		const static int key_pad_3 = glfwGetKeyScancode(GLFW_KEY_KP_3);
		const static int key_pad_4 = glfwGetKeyScancode(GLFW_KEY_KP_4);
		const static int key_pad_5 = glfwGetKeyScancode(GLFW_KEY_KP_5);
		const static int key_pad_6 = glfwGetKeyScancode(GLFW_KEY_KP_6);
		const static int key_pad_7 = glfwGetKeyScancode(GLFW_KEY_KP_7);
		const static int key_pad_8 = glfwGetKeyScancode(GLFW_KEY_KP_8);
		const static int key_pad_9 = glfwGetKeyScancode(GLFW_KEY_KP_9);

		//Tetris Guideline
		this->keybinds_a = Keybinds();
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_cw, key_up);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_cw, key_x);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_cw, key_pad_1);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_cw, key_pad_5);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_cw, key_pad_9);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_ccw, key_z);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_ccw, key_lctrl);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_ccw, key_pad_3);
		this->addKeyMapping(this->keybinds_a, &Keybinds::rotate_ccw, key_pad_7);
		this->addKeyMapping(this->keybinds_a, &Keybinds::move_left, key_left);
		this->addKeyMapping(this->keybinds_a, &Keybinds::move_left, key_pad_4);
		this->addKeyMapping(this->keybinds_a, &Keybinds::move_right, key_right);
		this->addKeyMapping(this->keybinds_a, &Keybinds::move_right, key_pad_6);
		this->addKeyMapping(this->keybinds_a, &Keybinds::hard_drop, key_space);
		this->addKeyMapping(this->keybinds_a, &Keybinds::hard_drop, key_pad_8);
		this->addKeyMapping(this->keybinds_a, &Keybinds::hold, key_c);
		this->addKeyMapping(this->keybinds_a, &Keybinds::hold, key_lshift);
		this->addKeyMapping(this->keybinds_a, &Keybinds::hold, key_pad_0);
		this->addKeyMapping(this->keybinds_a, &Keybinds::soft_drop, key_down);
		this->addKeyMapping(this->keybinds_a, &Keybinds::soft_drop, key_pad_2);
		this->addKeyMapping(this->keybinds_a, &Keybinds::keybinds_change, key_a);
		this->addKeyMapping(this->keybinds_a, &Keybinds::pause, key_esc);
		this->addKeyMapping(this->keybinds_a, &Keybinds::pause, key_f1);
		this->addKeyMapping(this->keybinds_a, &Keybinds::start, key_enter);
		this->addKeyMapping(this->keybinds_a, &Keybinds::mute_music, key_m);

		//my custom
		this->keybinds_b = Keybinds();
		this->addKeyMapping(this->keybinds_b, &Keybinds::rotate_cw, key_up);
		this->addKeyMapping(this->keybinds_b, &Keybinds::rotate_ccw, key_down);
		this->addKeyMapping(this->keybinds_b, &Keybinds::move_left, key_left);
		this->addKeyMapping(this->keybinds_b, &Keybinds::move_right, key_right);
		this->addKeyMapping(this->keybinds_b, &Keybinds::hard_drop, key_space);
		this->addKeyMapping(this->keybinds_b, &Keybinds::hold, key_c);
		this->addKeyMapping(this->keybinds_b, &Keybinds::soft_drop, key_v);
		this->addKeyMapping(this->keybinds_b, &Keybinds::keybinds_change, key_a);
		this->addKeyMapping(this->keybinds_b, &Keybinds::pause, key_esc);
		this->addKeyMapping(this->keybinds_b, &Keybinds::start, key_enter);
		this->addKeyMapping(this->keybinds_b, &Keybinds::mute_music, key_m);
	}

	void Tetris::addKeyMapping(Keybinds& keybinds, std::vector<int> Keybinds::* field, int key)
	{
		//i just like pointers to struct fields
		(keybinds.*field).push_back(key);
	}

	void Tetris::handleKeyPress(int key)
	{
		this->key_states[key] = true;

		const static auto any = [](int key, std::vector<int> keybinds) -> bool {
			for (int& keybind : keybinds)
			{
				if (key == keybind)
				{
					return true;
				}
			}

			return false;
		};

		if (any(key, this->keybinds->move_left))
		{
			this->auto_repeat_counter = 0;
			this->last_direction_key = key;
		}
		else if (any(key, this->keybinds->move_right))
		{
			this->auto_repeat_counter = 0;
			this->last_direction_key = key;
		}

		if (any(key, this->keybinds->keybinds_change))
		{
			this->keybinds_alternative = !this->keybinds_alternative;

			if (this->keybinds_alternative)
			{
				std::cout << "[TETRIS]: Keybinds changed to alternative!" << std::endl;
				this->keybinds = &this->keybinds_b;
			}
			else
			{
				std::cout << "[TETRIS]: Keybinds changed to default!" << std::endl;
				this->keybinds = &this->keybinds_a;
			}
		}

		if (any(key, this->keybinds->mute_music) && this->korobeiniki != nullptr)
		{
			this->mute_music = !this->mute_music;

			if (this->mute_music)
			{
				std::cout << "[TETRIS]: Music muted!" << std::endl;
				this->korobeiniki->setVolume(0.0f);
			}
			else
			{
				std::cout << "[TETRIS]: Music unmuted!" << std::endl;
				this->korobeiniki->setVolume(this->music_volume);
			}
		}

		if (any(key, this->keybinds->pause) && !this->death && this->game_started && this->countdown_counter == 0)
		{
			this->pause = !this->pause;
			if (this->pause)
			{
				if (this->korobeiniki != nullptr)
				{
					this->korobeiniki->setIsPaused(true);
				}
				std::cout << "[TETRIS]: Paused!" << std::endl;
			}
			else
			{
				if (this->korobeiniki != nullptr)
				{
					this->korobeiniki->setIsPaused(false);
				}
				this->startCountdown(3 * 60);
				std::cout << "[TETRIS]: Resumed!" << std::endl;
			}
		}

		if (!this->game_started && any(key, this->keybinds->start))
		{
			this->game_started = true;
		}

		if (this->death && any(key, this->keybinds->start))
		{
			this->restart();
		}
	}

	bool Tetris::isPressed(std::vector<int> Keybinds::* field)
	{
		for (int& k : this->keybinds->*field)
		{
			if (this->key_states[k] == true)
			{
				return true;
			}
		}

		return false;
	}

	void Tetris::unpress(std::vector<int> Keybinds::* field)
	{
		for (int& k : this->keybinds->*field)
		{
			this->key_states[k] = false;
		}
	}

	void Tetris::handleKeyRelease(int key)
	{
		this->key_states[key] = false;
	}

	Tetris::~Tetris()
	{
	}

	bool Tetris::isPaused()
	{
		return this->pause;
	}

	bool Tetris::isDead()
	{
		return this->death;
	}

	int Tetris::getScore()
	{
		return this->score_system.getPoints();
}
}
