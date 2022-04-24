#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "ShaderProgram.h"
#include <iostream>
#include "TextureObject.h"
#include <map>
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndicesBufferObject.h"
#include <algorithm>
#include "MathHelper.h"
#include "Rectangle.h"

using namespace OpenGLWrapper;

class FontRenderer
{
public:
	enum class SpecialSymbol
	{
		TETRIS_SINGLE,
		TETRIS_DOUBLE,
		TETRIS_TRIPLE,
		TETRIS_TETRIS,
		TETRIS_T_SPIN,
		TETRIS_T_SPIN_MINI,
		TETRIS_BACK_TO_BACK,
		TETRIS_PERFECT_CLEAR,
		TETRIS_COMBO,
		SMALL_0,
		SMALL_1,
		SMALL_2,
		SMALL_3,
		SMALL_4,
		SMALL_5,
		SMALL_6,
		SMALL_7,
		SMALL_8,
		SMALL_9,
		BIG_0,
		BIG_1,
		BIG_2,
		BIG_3,
		BIG_4,
		BIG_5,
		BIG_6,
		BIG_7,
		BIG_8,
		BIG_9,
		FANCY_0,
		FANCY_1,
		FANCY_2,
		FANCY_3,
		FANCY_4,
		FANCY_5,
		FANCY_6,
		FANCY_7,
		FANCY_8,
		FANCY_9,
		SMALL_X,
		BIG_X,
		SPACE_1PX,
		SPACE_2PX,
		SPACE_3PX,
		SPACE_4PX,
		SPACE_5PX,
		FULL_1PX,
		FULL_2PX,
		FULL_3PX,
		FULL_4PX,
		FULL_5PX,
		UPSIDE_QUESTION_MARK,
		UPSIDE_EXCLAMATION_MARK,
		TRADEMARK,
		COPYRIGHT,
		ESZETT,
		DEGREE,
		SMALL_PLUS_STAR,
		UPPER_TILDE,
		WIDE_PERCENTAGE_SQUARE,
		WIDE_PERCENTAGE_CIRCLE,
		HASH_BIG,
		HASH_SMALL,
		PLUS_BIG,
		MUSIC_MUTED,
		MUSIC,
		IDYLOS
	};

	enum class TextAlignment
	{
		TOP_LEFT,
		CENTER_LEFT,
		BOTTOM_LEFT,
		TOP_RIGHT,
		CENTER_RIGHT,
		BOTTOM_RIGHT,
		TOP_CENTER,
		CENTER_CENTER,
		BOTTOM_CENTER,

		CENTER = CENTER_CENTER,
		LEFT = BOTTOM_LEFT,
		RIGHT = BOTTOM_RIGHT
	};

protected:
	struct Glyph
	{
		enum class Type
		{
			CHARACTER,
			SPECIAL_SYMBOL
		} type;

		union
		{
			char character;
			SpecialSymbol symbol;
		};

		unsigned short width;	//render width, origin at 0
		unsigned short height;	//render height, origin at 0

		struct Origin
		{
			short x;
			short y;
		} origin;	//left most point on baseline (bottom of line height)

		struct Offset
		{
			short x;
			short y;
		} offset;

		unsigned short advance;	//glyph's hitbox, origin + advance = next letter's origin
	};

	class GlyphCache
	{
	public:
		std::map<SpecialSymbol, Glyph> special_symbols;
		std::map<char, Glyph> characters;

		GlyphCache();

		bool hasChar(const char& character)
		{
			return this->characters.find(character) != this->characters.end();
		}

		bool hasSpecialSymbol(const SpecialSymbol& symbol)
		{
			return this->special_symbols.find(symbol) != this->special_symbols.end();
		}

		Glyph& get(const char& character)
		{
			if (!this->hasChar(character))
			{
				std::cerr << "[FONT_RENDERER]: GlyphCache does not have the character \"" << character << "\"." << std::endl;
				throw character;
			}

			return this->characters[character];
		}

		Glyph& get(SpecialSymbol& symbol)
		{
			if (!this->hasSpecialSymbol(symbol))
			{
				std::cerr << "[FONT_RENDERER]: GlyphCache does not have the special symbol \"" << static_cast<int>(symbol) << "\"." << std::endl;
				throw symbol;
			}

			return this->special_symbols[symbol];
		}

		void addCharacter(char character, unsigned short width, unsigned short height, Glyph::Origin origin, Glyph::Offset offset, unsigned short advance)
		{
			Glyph glyph{};
			glyph.type = Glyph::Type::CHARACTER;
			glyph.character = character;
			glyph.width = width;
			glyph.height = height;
			glyph.origin = origin;
			glyph.offset = offset;
			glyph.advance = advance;
			this->characters[character] = glyph;
		}

		void addCharacter(char character, unsigned short width, unsigned short height, Glyph::Origin origin)
		{
			this->addCharacter(character, width, height, origin, { 0, 0 }, width + 1);
		}

		void addSpecialSymbol(SpecialSymbol symbol, unsigned short width, unsigned short height, Glyph::Origin origin, Glyph::Offset offset, unsigned short advance)
		{
			Glyph glyph{};
			glyph.type = Glyph::Type::SPECIAL_SYMBOL;
			glyph.symbol = symbol;
			glyph.width = width;
			glyph.height = height;
			glyph.origin = origin;
			glyph.offset = offset;
			glyph.advance = advance;
			this->special_symbols[symbol] = glyph;
		}

		void addSpecialSymbol(SpecialSymbol symbol, unsigned short width, unsigned short height, Glyph::Origin origin)
		{
			this->addSpecialSymbol(symbol, width, height, origin, { 0, 0 }, width + 1);
		}
	};

private:
	std::shared_ptr<TextureObject> texture;
	float texture_width = 0;
	float texture_height = 0;
	GlyphCache glyphs;
	std::unique_ptr<VertexArrayObject> VAO = std::make_unique<VertexArrayObject>();
	std::unique_ptr<VertexBufferObject> VBO = std::make_unique<VertexBufferObject>();
	std::unique_ptr<IndicesBufferObject> IBO = std::make_unique<IndicesBufferObject>();
	std::vector<glm::vec3> vertices;
	std::vector<glm::ivec3> indices;
	std::vector<glm::vec2> uv_coords;

	bool init();
public:
	friend std::string operator*(SpecialSymbol&& symbol);
	bool init(std::shared_ptr<TextureObject> texture);
	void renderText(std::shared_ptr<ShaderProgram> shader_program, const std::string text, glm::ivec2 pos, float size = 1.0f, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), TextAlignment align = TextAlignment::LEFT);
};

inline FontRenderer::GlyphCache::GlyphCache()
{
	this->addCharacter('0', 4, 7, { 45, 8 });
	this->addCharacter('1', 2, 7, { 51, 8 });
	this->addCharacter('2', 4, 7, { 55, 8 });
	this->addCharacter('3', 4, 7, { 60, 8 });
	this->addCharacter('4', 4, 7, { 65, 8 });
	this->addCharacter('5', 4, 7, { 70, 8 });
	this->addCharacter('6', 4, 7, { 75, 8 });
	this->addCharacter('7', 4, 7, { 80, 8 });
	this->addCharacter('8', 4, 7, { 85, 8 });
	this->addCharacter('9', 4, 7, { 90, 8 });

	this->addCharacter('A', 4, 7, { 1, 55 });
	this->addCharacter('B', 4, 7, { 6, 55 });
	this->addCharacter('C', 4, 7, { 11, 55 });
	this->addCharacter('D', 4, 7, { 16, 55 });
	this->addCharacter('E', 3, 7, { 21, 55 });
	this->addCharacter('F', 3, 7, { 25, 55 });
	this->addCharacter('G', 4, 7, { 29, 55 });
	this->addCharacter('H', 4, 7, { 34, 55 });
	this->addCharacter('I', 1, 7, { 39, 55 });
	this->addCharacter('J', 4, 7, { 41, 55 });
	this->addCharacter('K', 4, 7, { 46, 55 });
	this->addCharacter('L', 3, 7, { 51, 55 });
	this->addCharacter('M', 5, 7, { 55, 55 });
	this->addCharacter('N', 4, 7, { 61, 55 });
	this->addCharacter('O', 4, 7, { 66, 55 });
	this->addCharacter('P', 4, 7, { 71, 55 });
	this->addCharacter('Q', 5, 7, { 76, 55 });
	this->addCharacter('R', 4, 7, { 82, 55 });
	this->addCharacter('S', 4, 7, { 87, 55 });
	this->addCharacter('T', 3, 7, { 92, 55 });
	this->addCharacter('U', 4, 7, { 96, 55 });
	this->addCharacter('V', 5, 7, { 101, 55 });
	this->addCharacter('W', 5, 7, { 104, 47 });
	this->addCharacter('X', 5, 7, { 107, 55 });
	this->addCharacter('Y', 3, 7, { 113, 55 });
	this->addCharacter('Z', 5, 7, { 117, 55 });

	this->addCharacter('a', 4, 7, { 1, 55 });
	this->addCharacter('b', 4, 7, { 6, 55 });
	this->addCharacter('c', 4, 7, { 11, 55 });
	this->addCharacter('d', 4, 7, { 16, 55 });
	this->addCharacter('e', 3, 7, { 21, 55 });
	this->addCharacter('f', 3, 7, { 25, 55 });
	this->addCharacter('g', 4, 7, { 29, 55 });
	this->addCharacter('h', 4, 7, { 34, 55 });
	this->addCharacter('i', 1, 7, { 39, 55 });
	this->addCharacter('j', 4, 7, { 41, 55 });
	this->addCharacter('k', 4, 7, { 46, 55 });
	this->addCharacter('l', 3, 7, { 51, 55 });
	this->addCharacter('m', 5, 7, { 55, 55 });
	this->addCharacter('n', 4, 7, { 61, 55 });
	this->addCharacter('o', 4, 7, { 66, 55 });
	this->addCharacter('p', 4, 7, { 71, 55 });
	this->addCharacter('q', 5, 7, { 76, 55 });
	this->addCharacter('r', 4, 7, { 82, 55 });
	this->addCharacter('s', 4, 7, { 87, 55 });
	this->addCharacter('t', 3, 7, { 92, 55 });
	this->addCharacter('u', 4, 7, { 96, 55 });
	this->addCharacter('v', 5, 7, { 101, 55 });
	this->addCharacter('w', 5, 7, { 104, 47 });
	this->addCharacter('x', 5, 7, { 107, 55 });
	this->addCharacter('y', 3, 7, { 113, 55 });
	this->addCharacter('z', 5, 7, { 117, 55 });

	this->addCharacter('-', 3, 7, { 4, 65 }, { 0, -1 }, 4);
	this->addCharacter('_', 4, 7, { 17, 65 });
	this->addCharacter('.', 1, 7, { 22, 65 });
	this->addCharacter(',', 1, 8, { 26, 66 }, { 0, 1 }, 2);
	this->addCharacter(';', 1, 8, { 28, 66 }, { 0, 1 }, 2);
	this->addCharacter(':', 1, 7, { 44, 65 }, { 0, 1 }, 2);
	this->addCharacter('$', 5, 9, { 46, 66 }, { 0, 1 }, 6);
	this->addCharacter('¥', 5, 9, { 58, 67 }, { 0, 2 }, 5);
	this->addCharacter('Ê', 4, 9, { 63, 67 }, { 0, 2 }, 4);
	this->addCharacter('Ó', 4, 9, { 67, 65 });
	this->addCharacter('Œ', 4, 9, { 72, 65 });
	this->addCharacter('Æ', 4, 9, { 77, 65 });
	this->addCharacter('£', 4, 7, { 81, 65 }, { -1, 0 }, 4);
	this->addCharacter('¯', 5, 9, { 86, 65 });
	this->addCharacter('', 5, 9, { 92, 65 });
	this->addCharacter('Ñ', 4, 9, { 98, 65 });
	this->addCharacter('+', 3, 7, { 103, 65 });

	this->addCharacter(' ', 2, 7, { 3, 77 });
	this->addCharacter('{', 3, 7, { 21, 77 });
	this->addCharacter('}', 3, 7, { 25, 77 });
	this->addCharacter('<', 4, 7, { 29, 77 });
	this->addCharacter('>', 4, 7, { 34, 77 });
	this->addCharacter('?', 5, 7, { 39, 77 });
	this->addCharacter('!', 1, 7, { 51, 77 });
	this->addCharacter('`', 2, 7, { 55, 77 });
	this->addCharacter('^', 5, 7, { 58, 77 });
	this->addCharacter('~', 6, 7, { 64, 77 });
	this->addCharacter('(', 3, 7, { 101, 77 });
	this->addCharacter(')', 3, 7, { 105, 77 });
	this->addCharacter('[', 3, 7, { 109, 77 });
	this->addCharacter(']', 3, 7, { 113, 77 });

	this->addCharacter('*', 3, 7, { 5, 89 });
	this->addCharacter('|', 1, 7, { 13, 89 });
	this->addCharacter('=', 2, 7, { 33, 89 });
	this->addCharacter('/', 3, 7, { 45, 89 });
	this->addCharacter('\\', 3, 7, { 54, 89 });
	this->addCharacter('%', 5, 7, { 79, 89 });
	this->addCharacter('\'', 1, 7, { 85, 89 });
	this->addCharacter('"', 3, 7, { 87, 89 });
	this->addCharacter('#', 5, 7, { 99, 89 });
	this->addCharacter('@', 7, 7, { 112, 89 });
	this->addCharacter('&', 5, 7, { 120, 89 });

	this->addSpecialSymbol(SpecialSymbol::TETRIS_SINGLE, 24, 7, { 1, 15 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_DOUBLE, 27, 7, { 1, 23 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_TRIPLE, 23, 7, { 1, 31 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_TETRIS, 23, 7, { 1, 39 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_T_SPIN, 23, 7, { 1, 47 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_T_SPIN_MINI, 41, 7, { 1, 47 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_BACK_TO_BACK, 54, 7, { 41, 23 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_COMBO, 25, 7, { 100, 31 });
	this->addSpecialSymbol(SpecialSymbol::TETRIS_PERFECT_CLEAR, 56, 7, { 37, 31 });

	this->addSpecialSymbol(SpecialSymbol::BIG_X, 5, 7, { 95, 8 });

	this->addSpecialSymbol(SpecialSymbol::FANCY_0, 4, 7, { 52, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_1, 4, 7, { 57, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_2, 4, 7, { 62, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_3, 4, 7, { 67, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_4, 4, 7, { 72, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_5, 4, 7, { 77, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_6, 4, 7, { 82, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_7, 4, 7, { 87, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_8, 4, 7, { 92, 40 });
	this->addSpecialSymbol(SpecialSymbol::FANCY_9, 4, 7, { 97, 40 });

	this->addSpecialSymbol(SpecialSymbol::SMALL_0, 3, 5, { 1, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_1, 3, 5, { 5, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_2, 3, 5, { 9, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_3, 3, 5, { 13, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_4, 3, 5, { 17, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_5, 3, 5, { 21, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_6, 3, 5, { 25, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_7, 3, 5, { 29, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_8, 3, 5, { 33, 6 });
	this->addSpecialSymbol(SpecialSymbol::SMALL_9, 3, 5, { 37, 6 });

	this->addSpecialSymbol(SpecialSymbol::BIG_0, 4, 7, { 45, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_1, 2, 7, { 51, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_2, 4, 7, { 55, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_3, 4, 7, { 60, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_4, 4, 7, { 65, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_5, 4, 7, { 70, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_6, 4, 7, { 75, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_7, 4, 7, { 80, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_8, 4, 7, { 85, 8 });
	this->addSpecialSymbol(SpecialSymbol::BIG_9, 4, 7, { 90, 8 });

	this->addSpecialSymbol(SpecialSymbol::MUSIC_MUTED, 15, 15, { 1, 108 });
	this->addSpecialSymbol(SpecialSymbol::MUSIC, 15, 15, { 18, 108 });

	this->addSpecialSymbol(SpecialSymbol::IDYLOS, 33, 19, { 36, 112 }, { -4, 7 }, 22);
}
