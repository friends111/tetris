#include "Rectangle.h"

namespace Geometry
{
	void Rectangle::initFrom3D(glm::vec3 a, glm::vec3 b)
	{
		float x_min = std::min(a.x, b.x);
		float x_max = std::max(a.x, b.x);
		float y_min = std::min(a.y, b.y);
		float y_max = std::max(a.y, b.y);
		float z_min = std::min(a.z, b.z);
		float z_max = std::max(a.z, b.z);
		this->vertices.push_back(glm::vec3(x_min, y_min, a.z));
		this->vertices.push_back(glm::vec3(x_min, y_max, a.z));
		this->vertices.push_back(glm::vec3(x_max, y_max, b.z));
		this->vertices.push_back(glm::vec3(x_max, y_min, b.z));
		//these vertices might not be correct, i haven't tested it because i'm not using 3rd dimension
		this->ready_to_draw = this->init();
	}

	void Rectangle::initFrom2D(glm::vec2 a, glm::vec2 b)
	{
		float x_min = std::min(a.x, b.x);
		float x_max = std::max(a.x, b.x);
		float y_min = std::min(a.y, b.y);
		float y_max = std::max(a.y, b.y);
		this->vertices.push_back(glm::vec3(x_min, y_min, 0.0f));
		this->vertices.push_back(glm::vec3(x_min, y_max, 0.0f));
		this->vertices.push_back(glm::vec3(x_max, y_max, 0.0f));
		this->vertices.push_back(glm::vec3(x_max, y_min, 0.0f));
		this->ready_to_draw = this->init();
	}
}
