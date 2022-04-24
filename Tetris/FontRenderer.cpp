#include "FontRenderer.h"

bool FontRenderer::init()
{
	//Create VAO if not ready
	if (!this->VAO->isReady())
	{
		if (!this->VAO->create())
		{
			std::cerr << "[ERROR]: FontRenderer::VAO creation failed!" << std::endl;
			return false;
		}
	}
	this->VAO->bind();

	//Create VBO
	if (!this->VBO->isReady())
	{
		if (!this->VBO->create())
		{
			std::cerr << "[ERROR]: FontRenderer::VBO creation failed!" << std::endl;
			return false;
		}
	}
	this->VBO->bind();
	this->VBO->setUsage(GL_DYNAMIC_DRAW);
	int vertices_size = this->vertices.size() * sizeof(decltype(this->vertices)::value_type);
	int uv_size = this->uv_coords.size() * sizeof(decltype(this->uv_coords)::value_type);
	this->VBO->allocate(vertices_size + uv_size);
	this->VBO->write(0, this->vertices);
	this->VBO->write(vertices_size, this->uv_coords);

	//Create IBO if not ready
	if (!this->IBO->isReady())
	{
		if (!this->IBO->create())
		{
			std::cerr << "[ERROR]: FontRenderer::IBO creation failed!" << std::endl;
			return false;
		}
	}
	this->IBO->bind();
	this->IBO->setUsage(GL_DYNAMIC_DRAW);
	this->IBO->allocate(this->indices);

	//Check if texture is ready
	if (this->texture == nullptr || !this->texture->isCreated())
	{
		std::cerr << "[ERROR]: FontRenderer::texture is nullptr or not created!" << std::endl;
		return false;
	}
	this->texture->bind();

	//Vertex attributes
	this->VAO->addAttribute(3, GL_FLOAT, GL_FALSE, 0, vertices_size);
	this->VAO->addAttribute(2, GL_FLOAT, GL_FALSE, 0, uv_size);

	this->VAO->release();
	this->VBO->release();
	this->IBO->release();
	this->texture->release();

	return true;
}

std::string operator*(FontRenderer::SpecialSymbol&& symbol)
{
	return "%" + std::to_string(static_cast<int>(symbol)) + "%";
}

bool FontRenderer::init(std::shared_ptr<TextureObject> texture)
{
	this->texture = texture;
	this->texture_width = static_cast<float>(this->texture->getWidth());
	this->texture_height = static_cast<float>(this->texture->getHeight());
	return this->init();
}

void FontRenderer::renderText(std::shared_ptr<ShaderProgram> shader_program, const std::string text, glm::ivec2 pos, float size, glm::vec4 color, TextAlignment align)
{
	if (shader_program == nullptr)
	{
		std::cerr << "[FONT_RENDERER]: ShaderProgram is nullptr." << std::endl;
		return;
	}

	if (this->texture == nullptr)
	{
		std::cerr << "[FONT_RENDERER]: Texture is nullptr." << std::endl;
		return;
	}

	this->vertices.clear();
	this->indices.clear();
	this->uv_coords.clear();

	int advance = 0;
	int i = 0;
	bool reading_special_symbol = false;
	bool special_finished = false;
	std::string special_symbol_str = "";
	SpecialSymbol special_symbol = SpecialSymbol::TETRIS_SINGLE;
	short baseline = 0;

	for (const char& character : text)
	{
		if (character == '%' && !reading_special_symbol)
		{
			reading_special_symbol = true;
			continue;
		}
		else if (reading_special_symbol && character == '%')
		{
			reading_special_symbol = false;
			special_finished = true;
		}
		else if (reading_special_symbol)
		{
			special_symbol_str += character;
			continue;
		}

		if (special_finished)
		{
			if (special_symbol_str.empty())	//%% -> %
			{
				special_finished = false;
			}
			else
			{
				int special_symbol_value = std::stoi(special_symbol_str);
				special_symbol = static_cast<SpecialSymbol>(special_symbol_value);
			}
		}

		Glyph& glyph = special_finished ? this->glyphs.get(special_symbol) : this->glyphs.get(character);
		special_symbol_str = "";
		special_finished = false;

		if (glyph.offset.y != 0)
		{
			baseline = std::min(baseline, glyph.offset.y);
		}

		this->vertices.push_back({ size * (advance + glyph.offset.x), size * glyph.offset.y, 0.0f });
		this->vertices.push_back({ size * (advance + glyph.offset.x), -(size * (glyph.height - glyph.offset.y)), 0.0f });
		this->vertices.push_back({ size * (advance + glyph.width + glyph.offset.x), -(size * (glyph.height - glyph.offset.y)), 0.0f });
		this->vertices.push_back({ size * (advance + glyph.width + glyph.offset.x), size * glyph.offset.y, 0.0f });
		this->indices.push_back({ i + 0, i + 1, i + 2 });
		this->indices.push_back({ i + 2, i + 3, i + 0 });
		this->uv_coords.push_back({ glyph.origin.x / this->texture_width, glyph.origin.y / this->texture_height });
		this->uv_coords.push_back({ glyph.origin.x / this->texture_width, (glyph.origin.y - glyph.height) / this->texture_height });
		this->uv_coords.push_back({ (glyph.origin.x + glyph.width) / this->texture_width, (glyph.origin.y - glyph.height) / this->texture_height });
		this->uv_coords.push_back({ (glyph.origin.x + glyph.width) / this->texture_width, glyph.origin.y / this->texture_height });

		advance += glyph.advance;
		i += 4;
	}

	glm::mat4x4 transform = glm::mat4x4(1.0f);

#ifdef __cpp_nontype_template_parameter_auto
	auto min_x_it = std::min_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<&glm::vec3::x>);
	auto max_x_it = std::max_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<&glm::vec3::x>);
	auto min_y_it = std::min_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<&glm::vec3::y>);
	auto max_y_it = std::max_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<&glm::vec3::y>);
#else
	auto min_x_it = std::min_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<glm::vec3, float, &glm::vec3::x>);
	auto max_x_it = std::max_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<glm::vec3, float, &glm::vec3::x>);
	auto min_y_it = std::min_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<glm::vec3, float, &glm::vec3::y>);
	auto max_y_it = std::max_element(this->vertices.begin(), this->vertices.end(), MathHelper::memberCompare<glm::vec3, float, &glm::vec3::y>);
#endif
	
	int diff_x = static_cast<int>((*max_x_it - *min_x_it).x);	//text bounding box's width
	int diff_y = static_cast<int>((*max_y_it - *min_y_it).y);	//text bounding box's height

	if (align == TextAlignment::BOTTOM_LEFT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x, pos.y, 0.0f));
	}
	else if (align == TextAlignment::BOTTOM_RIGHT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x, pos.y, 0.0f));
	}
	else if (align == TextAlignment::BOTTOM_CENTER)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x / 2.0f, pos.y, 0.0f));
	}
	else if (align == TextAlignment::TOP_LEFT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x, pos.y + diff_y, 0.0f));
	}
	else if (align == TextAlignment::TOP_RIGHT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x, pos.y + diff_y, 0.0f));
	}
	else if (align == TextAlignment::TOP_CENTER)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x / 2.0f, pos.y + diff_y, 0.0f));
	}
	else if (align == TextAlignment::CENTER_LEFT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x, pos.y + diff_y / 2.0f, 0.0f));
	}
	else if (align == TextAlignment::CENTER_RIGHT)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x, pos.y + diff_y / 2.0f, 0.0f));
	}
	else if (align == TextAlignment::CENTER_CENTER)
	{
		transform = glm::translate(transform, glm::vec3(pos.x - diff_x / 2.0f, pos.y + diff_y / 2.0f, 0.0f));
	}
	else
	{
		std::cerr << "[FONT_RENDERER]: Unsupported TextAlignment." << std::endl;
		return;
	}

	shader_program->setUniformValue("model_matrix", transform);
	shader_program->setUniformValue("color", color);
	shader_program->setUniformValue("textured", true);

	this->VAO->bind();

	this->VBO->bind();
	int vertices_size = this->vertices.size() * sizeof(decltype(this->vertices)::value_type);
	int uv_size = this->uv_coords.size() * sizeof(decltype(this->uv_coords)::value_type);
	this->VBO->allocate(vertices_size + uv_size);
	this->VBO->write(0, this->vertices);
	this->VBO->write(vertices_size, this->uv_coords);

	this->VAO->modifyAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->VAO->modifyAttribute(1, 2, GL_FLOAT, GL_FALSE, 0, vertices_size);

	this->IBO->bind();
	this->IBO->allocate(this->indices);

	this->texture->bind();

	glDrawElements(GL_TRIANGLES, this->indices.size() * 3, GL_UNSIGNED_INT, 0);

	this->VAO->release();
	this->IBO->release();
	this->texture->release();

	shader_program->setUniformValue("textured", false);
}
