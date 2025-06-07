#pragma once

#include "../global/base.h"

namespace ff {

	class Sphere {
	public:
		using Ptr = std::shared_ptr<Sphere>;	
		static Ptr create(const glm::vec3& center, float radius)
		{
			return std::make_shared<Sphere>(center, radius);
		}


		Sphere(const glm::vec3& center, float radius) noexcept
		{
			m_center = center;
			m_radius = radius;
		}

		~Sphere() noexcept {}

		//应用在跟随物体进行Matrix变换的时候
		void applyMatrix4(const glm::mat4 matrix) noexcept
		{
			m_center = glm::vec3(matrix * glm::vec4(m_center, 1.0));

			//对于半径，只会收到scale缩放影响，我们只需要烤炉三个scale当中最大的
			float scaleX = glm::length(glm::vec3(matrix[0]));
			float scaleY = glm::length(glm::vec3(matrix[1]));
			float scaleZ = glm::length(glm::vec3(matrix[2]));

			float maxScale = std::max(std::max(scaleX, scaleY), scaleZ);

			m_radius *= maxScale;
		}

		void copy(const Sphere::Ptr& other)
		{
			m_center = other->m_center;
			m_radius = other->m_radius;
		}

	public:
		glm::vec3	m_center = glm::vec3(0.0f);
		float		m_radius{ 0.0f };
	
	};
}