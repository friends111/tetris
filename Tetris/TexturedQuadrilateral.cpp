#include "TexturedQuadrilateral.h"

namespace Geometry
{
	bool TexturedQuadrilateral::init()
	{
		//Create VAO if not ready
		if (!this->VAO->isReady())
		{
			if (!this->VAO->create())
			{
				std::cerr << "[ERROR]: TexturedQuadrilateral::VAO creation failed!" << std::endl;
				return false;
			}
		}
		this->VAO->bind();

		//Create VBO
		if (!this->VBO->isReady())
		{
			if (!this->VBO->create())
			{
				std::cerr << "[ERROR]: TexturedQuadrilateral::VBO creation failed!" << std::endl;
				return false;
			}
		}
		this->VBO->bind();
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
				std::cerr << "[ERROR]: TexturedQuadrilateral::IBO creation failed!" << std::endl;
				return false;
			}
		}
		this->IBO->bind();
		this->IBO->allocate(this->indices);

		//Check if texture is ready
		if (this->texture == nullptr || !this->texture->isCreated())
		{
			std::cerr << "[ERROR]: TexturedQuadrilateral::texture is nullptr or not created!" << std::endl;
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

	void TexturedQuadrilateral::draw(std::shared_ptr<ShaderProgram> shader_program)
	{
		if (shader_program == nullptr)
		{
			return;
		}

		if (!this->ready_to_draw)
		{
			return;
		}

		shader_program->setUniformValue("color", this->color);
		shader_program->setUniformValue("model_matrix", this->getModelMatrix());
		shader_program->setUniformValue("textured", true);

		this->VAO->bind();
		this->IBO->bind();
		this->texture->bind();

		glDrawElements(GL_TRIANGLES, this->indices.size() * 3, GL_UNSIGNED_INT, 0);

		this->VAO->release();
		this->IBO->release();
		this->texture->release();

		shader_program->setUniformValue("textured", false);
	}

	void TexturedQuadrilateral::initFrom3D(std::shared_ptr<TextureObject> texture, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
	{
		this->texture = texture;
		this->Quadrilateral::initFrom3D(p1, p2, p3, p4);
	}

	void TexturedQuadrilateral::initFrom2D(std::shared_ptr<TextureObject> texture, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
	{
		this->texture = texture;
		this->Quadrilateral::initFrom2D(p1, p2, p3, p4);
	}
}
