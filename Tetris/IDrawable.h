#pragma once
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndicesBufferObject.h"
#include <iostream>
#include "ShaderProgram.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

namespace Geometry
{
	using namespace OpenGLWrapper;

	class IDrawable
	{
	protected:
		glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		virtual glm::mat4x4 getTranslationMatrix();
		virtual glm::mat4x4 getRotationMatrix();
		virtual glm::mat4x4 getScaleMatrix();
		virtual glm::mat4x4 getModelMatrix();
	public:
		virtual void draw(std::shared_ptr<ShaderProgram> shader_program) = 0;
		virtual bool init() = 0;
		virtual void setPos(float xf, float y, float z = 0.0f);
		virtual void setPos(glm::vec3 pos);
		virtual void setScale(float x, float y, float z = 1.0f);
		virtual void setScale(float s);
		virtual void setScale(glm::vec3 scale);
		virtual void setRotation(float x, float y, float z, float w);
		virtual void setRotation(glm::quat rotation);
		virtual void setColor(float red, float green, float blue, float alpha = 1.0f);
		virtual void setColor(glm::vec4 color);
		virtual void destroy() = 0;
		virtual ~IDrawable() = default;
	};

	inline glm::mat4x4 IDrawable::getTranslationMatrix()
	{
		return glm::translate(glm::mat4x4(1.0f), this->position);
	}

	inline glm::mat4x4 IDrawable::getRotationMatrix()
	{
		return glm::toMat4(this->rotation);
	}

	inline glm::mat4x4 IDrawable::getScaleMatrix()
	{
		return glm::scale(glm::mat4x4(1.0f), this->scale);
	}

	inline glm::mat4x4 IDrawable::getModelMatrix()
	{
		return this->getTranslationMatrix() * this->getRotationMatrix() * this->getScaleMatrix();
	}

	inline void IDrawable::setPos(float x, float y, float z)
	{
		this->position = glm::vec3(x, y, z);
	}

	inline void IDrawable::setPos(glm::vec3 pos)
	{
		this->position = pos;
	}

	inline void IDrawable::setColor(float red, float green, float blue, float alpha)
	{
		this->color = glm::vec4(red, green, blue, alpha);
	}

	inline void IDrawable::setColor(glm::vec4 color)
	{
		this->color = color;
	}

	inline void IDrawable::setScale(float x, float y, float z)
	{
		this->scale = glm::vec3(x, y, z);
	}

	inline void IDrawable::setScale(float s)
	{
		this->scale = glm::vec3(s, s, s);
	}

	inline void IDrawable::setScale(glm::vec3 scale)
	{
		this->scale = scale;
	}

	inline void IDrawable::setRotation(float x, float y, float z, float w)
	{
		this->rotation = glm::quat(w, x, y, z);
	}

	inline void IDrawable::setRotation(glm::quat rot)
	{
		this->rotation = rot;
	}
}
