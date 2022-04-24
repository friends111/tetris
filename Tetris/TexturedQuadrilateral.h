#pragma once
#include "Quadrilateral.h"
#include "TextureObject.h"

namespace Geometry
{
	class TexturedQuadrilateral : public Quadrilateral
	{
	protected:
		std::vector<glm::vec2> uv_coords = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f)
		};
		bool init() override;
		std::shared_ptr<TextureObject> texture;

	public:
		void draw(std::shared_ptr<ShaderProgram> shader_program) override;

		void initFrom3D(std::shared_ptr<TextureObject> texture, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
		void initFrom2D(std::shared_ptr<TextureObject> texture, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4);

	};
}
