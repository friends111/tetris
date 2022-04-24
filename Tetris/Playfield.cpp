#include "Playfield.h"

namespace Tetris
{
    int Playfield::getPlayableWidth()
    {
        return this->width - this->playable_width_offset;
    }

    int Playfield::getPlayableHeight()
    {
        return this->height - this->playable_height_offset;
    }

    void Playfield::initGridDrawables()
    {
        for (int y = 0; y < this->height; y++)
        {
            std::vector<std::unique_ptr<Geometry::Rectangle>> vec;
            for (int x = 0; x < this->width; x++)
            {
                std::unique_ptr<Geometry::Rectangle> cell = std::make_unique<Geometry::Rectangle>();
                cell->initFrom2D(glm::vec2(-0.5f, -0.5f), glm::vec2(0.5f, 0.5f));
                cell->setPos(glm::vec3(this->gridToScreenCoordinates(x, y), 0.0f));
                cell->setScale(this->cell_size);
                //glm::vec4 color = glm::vec4(0.5 + std::rand() / (float)RAND_MAX, 0.5 + std::rand() / (float)RAND_MAX, 0.5 + std::rand() / (float)RAND_MAX, 0.5f
                cell->setColor(0.1f, 0.1f, 0.1f, 1.0f);
                //this->grid_squares[y][x] = cell;
                vec.push_back(std::move(cell));
            }
            this->grid_squares.push_back(std::move(vec));
        }

        glm::vec2 board_top_left = this->gridToScreenCoordinates(0, this->getPlayableHeight());
        glm::vec2 board_bottom_right = this->gridToScreenCoordinates(this->width - 1, this->height - 1);
        this->board_top_boundary = board_top_left.y - 32.0f;
        this->board_background.initFrom2D(board_top_left - 32.0f, board_bottom_right + 32.0f);
        this->board_background.setPos(0, 0);
        this->board_background.setColor(0.2f, 0.2f, 0.2f, 1.0f);
        this->board_background_border.initFrom2D(board_top_left - 32.0f - 1.0f, board_bottom_right + 32.0f + 1.0f);
        this->board_background_border.setPos(0, 0);
        this->board_background_border.setColor(0.15f, 0.15f, 0.15f, 1.0f);

        glm::vec2 hold_top_left = this->gridToScreenCoordinates(-8, 21);
        glm::vec2 hold_bottom_right = this->gridToScreenCoordinates(-4, 24);
        this->hold_background.initFrom2D(hold_top_left - 32.0f - 1.0f, hold_bottom_right + 32.0f + 1.0f);
        this->hold_background.setPos(0, 0);
        this->hold_background.setColor(0.2f, 0.2f, 0.2f, 1.0f);
        this->hold_background_border.initFrom2D(hold_top_left - 32.0f - 2.0f, hold_bottom_right + 32.0f + 2.0f);
        this->hold_background_border.setPos(0, 0);
        this->hold_background_border.setColor(0.15f, 0.15f, 0.15f, 1.0f);

        glm::vec2 queue_top_left = this->gridToScreenCoordinates(14, 21);
        glm::vec2 queue_bottom_right = this->gridToScreenCoordinates(18, 38);
        this->queue_background.initFrom2D(queue_top_left - 32.0f - 1.0f, queue_bottom_right + 32.0f + 1.0f);
        this->queue_background.setPos(0, 0);
        this->queue_background.setColor(0.2f, 0.2f, 0.2f, 1.0f);
        this->queue_background_border.initFrom2D(queue_top_left - 32.0f - 2.0f, queue_bottom_right + 32.0f + 2.0f);
        this->queue_background_border.setPos(0, 0);
        this->queue_background_border.setColor(0.15f, 0.15f, 0.15f, 1.0f);

        glm::vec2 score_top_left = this->gridToScreenCoordinates(-8, 30);
        glm::vec2 score_bottom_right = this->gridToScreenCoordinates(-4, 38);
        this->score_background.initFrom2D(score_top_left - 32.0f - 1.0f, score_bottom_right + 32.0f + 1.0f);
        this->score_background.setPos(0, 0);
        this->score_background.setColor(0.2f, 0.2f, 0.2f, 1.0f);
        this->score_background_border.initFrom2D(score_top_left - 32.0f - 2.0f, score_bottom_right + 32.0f + 2.0f);
        this->score_background_border.setPos(0, 0);
        this->score_background_border.setColor(0.15f, 0.15f, 0.15f, 1.0f);
    }

    Playfield::Playfield(int screen_width, int screen_height) : screen_width(screen_width), screen_height(screen_height)
    {
        this->initGridDrawables();
    }

    glm::vec2 Playfield::gridToScreenCoordinates(glm::ivec2 grid_pos)
    {
        return gridToScreenCoordinates(grid_pos.x, grid_pos.y);
    }

    glm::vec2 Playfield::gridToScreenCoordinates(int grid_x, int grid_y)
    {
        grid_x -= this->playable_width_offset;
        grid_y -= this->playable_height_offset;
        float playfield_width_half = this->getPlayableWidth() / 2.0f;
        float playfield_height_half = (this->getPlayableHeight()) / 2.0f;
        float cell = this->cell_size + this->cell_spacing;

        float screen_x = (this->screen_width / 2.0f) - (playfield_width_half * cell) + (cell / 2.0f);    // top left corner of playfield
        float screen_y = (this->screen_height / 2.0f) - (playfield_height_half * cell) + (cell / 2.0f);

        screen_x += cell * grid_x; // nth cell position + space between them
        screen_y += cell * grid_y;

        return glm::vec2(screen_x, screen_y);
    }

    float Playfield::getCellSize()
    {
        return this->cell_size;
    }

    float Playfield::getBoardTopBoundary()
    {
        return this->board_top_boundary;
    }

    glm::ivec2 Playfield::getSize()
    {
        return glm::ivec2(this->width, this->height);
    }

    void Playfield::draw(std::shared_ptr<ShaderProgram> shader_program)
    {
        this->board_background_border.draw(shader_program);
        this->board_background.draw(shader_program);
        this->hold_background_border.draw(shader_program);
        this->hold_background.draw(shader_program);
        this->queue_background_border.draw(shader_program);
        this->queue_background.draw(shader_program);
        this->score_background_border.draw(shader_program);
        this->score_background.draw(shader_program);

        for (int y = this->getPlayableHeight(); y < this->height; y++)
        {
            for (int x = 0; x < this->getPlayableWidth(); x++)
            {
                this->grid_squares[y][x]->draw(shader_program);
            }
        }
    }
}
