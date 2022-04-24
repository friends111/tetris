#pragma once
#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace MathHelper
{
    // https://stackoverflow.com/a/29447699
    template<class T>
	static T round(T a)
    {
        static_assert(std::is_floating_point<T>::value, "round<T>: T must be floating point");

        return (a > 0) ? std::floor(a + static_cast<T>(0.5)) : std::ceil(a - static_cast<T>(0.5));
    }

    template<class T>
	static T round(T a, int places)
    {
        static_assert(std::is_floating_point<T>::value, "round<T>: T must be floating point");

        const T shift = pow(static_cast<T>(10.0), places);

        return round(a * shift) / shift;
    }

	//comparison function for glm::vec3
	static bool vec3_cmp(const glm::vec3& a, const glm::vec3& b)
	{
		if (a.x <= b.x &&
			a.y <= b.y &&
			a.z <= b.z)
		{
			return true;
		}

		return false;
	}

	template<typename T> bool any(T value)
	{
		return value;
	}

	//returns true if any of the given arguments (which can be any number of except for 0) evaluates true
	template<typename T, typename... Ts> bool any(T first_value, Ts... values)
	{
		return first_value || any(values...);
	}

#ifdef __cpp_nontype_template_parameter_auto	// == 201606L ?
	template <class Class, class Type>
	Class classof(Type Class::* field);

	template<auto field>
	static bool memberCompare(decltype(classof(field)) const& lhs, decltype(classof(field)) const& rhs)
	{
		return lhs.*field < rhs.*field;
	};
#else
	template<class T, typename V, V T::* field>
	static bool memberCompare(T const& lhs, T const& rhs)
	{
		return lhs.*field < rhs.*field;
	};
#endif

	struct Point
	{
		int x;
		int y;
		bool operator<(const Point& right) const	//needed for comparison for containers
		{
			if (this->x != right.x)
			{
				return this->x < right.x;
			}

			return this->y < right.y;
		}
		Point(glm::ivec2 data) : x(data.x), y(data.y) {};
		Point(int x, int y) : x(x), y(y) {};
		Point() : x(0), y(0) {};
		operator glm::ivec2() const
		{
			return glm::ivec2(x, y);
		}
	};
};
