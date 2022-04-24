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
	/// <summary>
	/// Convex quadrilateral
	/// </summary>
	class Quadrilateral : public IDrawable
	{
	protected:
		std::unique_ptr<VertexArrayObject> VAO = std::make_unique<VertexArrayObject>();
		std::unique_ptr<VertexBufferObject> VBO = std::make_unique<VertexBufferObject>();
		std::unique_ptr<IndicesBufferObject> IBO = std::make_unique<IndicesBufferObject>();
		std::vector<glm::vec3> vertices;
		std::vector<glm::ivec3> indices = {
			glm::ivec3(0, 1, 2),
			glm::ivec3(2, 3, 0)
		};
		bool ready_to_draw = false;
		bool init() override;
	public:
		Quadrilateral();
		void draw(std::shared_ptr<ShaderProgram> shader_program) override;

		/// <summary>
		/// Points here must be in order, meaning p1 connects to p2, p2 to p3, p3 to p4 and p4 to p1.
		/// </summary>
		void initFrom3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);

		/// <summary>
		/// Points here must be in order, meaning p1 connects to p2, p2 to p3, p3 to p4 and p4 to p1.
		/// </summary>
		void initFrom2D(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4);
		void destroy() override;
		~Quadrilateral() override;
	};
}
