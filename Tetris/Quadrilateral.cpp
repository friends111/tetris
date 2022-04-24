#include "Quadrilateral.h"

namespace Geometry
{
	Quadrilateral::Quadrilateral()
	{
	}

	void Quadrilateral::initFrom3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
	{
		this->vertices.push_back(p1);
		this->vertices.push_back(p2);
		this->vertices.push_back(p3);
		this->vertices.push_back(p4);
		this->ready_to_draw = this->init();
	}

	void Quadrilateral::initFrom2D(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
	{
		this->vertices.push_back(glm::vec3(p1, 0.0f));
		this->vertices.push_back(glm::vec3(p2, 0.0f));
		this->vertices.push_back(glm::vec3(p3, 0.0f));
		this->vertices.push_back(glm::vec3(p4, 0.0f));
		this->ready_to_draw = this->init();
	}

	void Quadrilateral::destroy()
	{
	}

	Quadrilateral::~Quadrilateral()
	{
		this->destroy();
	}

	bool Quadrilateral::init()
	{
		//Create VAO if not ready
		if (!this->VAO->isReady())
		{
			if (!this->VAO->create())
			{
				std::cerr << "[ERROR]: Quadrilateral::VAO creation failed!" << std::endl;
				return false;
			}
		}
		this->VAO->bind();

		//Create VBO
		if (!this->VBO->isReady())
		{
			if (!this->VBO->create())
			{
				std::cerr << "[ERROR]: Quadrilateral::VBO creation failed!" << std::endl;
				return false;
			}
		}
		this->VBO->bind();
		int vertices_size = this->vertices.size() * sizeof(decltype(this->vertices)::value_type);
		this->VBO->allocate(this->vertices);

		//Create IBO if not ready
		if (!this->IBO->isReady())
		{
			if (!this->IBO->create())
			{
				std::cerr << "[ERROR]: Quadrilateral::IBO creation failed!" << std::endl;
				return false;
			}
		}
		this->IBO->bind();
		this->IBO->allocate(this->indices);

		//Vertex attributes
		this->VAO->addAttribute(3, GL_FLOAT, GL_FALSE, 0, vertices_size);

		this->VAO->release();
		this->VBO->release();
		this->IBO->release();

		return true;
	}

	void Quadrilateral::draw(std::shared_ptr<ShaderProgram> shader_program)
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

		this->VAO->bind();
		this->IBO->bind();

		glDrawElements(GL_TRIANGLES, this->indices.size() * 3, GL_UNSIGNED_INT, 0);

		this->VAO->release();
		this->IBO->release();
	}
}
