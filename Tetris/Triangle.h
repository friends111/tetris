#pragma once
#include "IDrawable.h"
#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include "ShaderProgram.h"

namespace Geometry
{
	class Triangle : public IDrawable
	{
	protected:
		std::unique_ptr<VertexArrayObject> VAO = std::make_unique<VertexArrayObject>();
		std::unique_ptr<VertexBufferObject> VBO = std::make_unique<VertexBufferObject>();
		std::unique_ptr<IndicesBufferObject> IBO = std::make_unique<IndicesBufferObject>();
		std::vector<glm::vec3> vertices;
		std::vector<glm::ivec3> indices = {
			glm::ivec3(0, 1, 2)
		};
		bool ready_to_draw = false;
		bool init() override;
	public:
		Triangle();
		void draw(std::shared_ptr<ShaderProgram> shader_program) override;
		void initFrom3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
		void initFrom2D(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
		void destroy() override;
		~Triangle() override;
	};
}
