#pragma once
#include "Quadrilateral.h"

namespace Geometry
{
	class Rectangle : public Quadrilateral
	{
	public:
		void initFrom3D(glm::vec3 a, glm::vec3 b);
		void initFrom2D(glm::vec2 a, glm::vec2 b);
	};
}
