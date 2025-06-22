/**
 * @class Sphere
 * @brief 表示三维空间中的球体，用于包围体计算、碰撞检测等场景。
 *
 * `Sphere` 用于表示一个具有中心点和半径的几何球体，常用于包围体测试（Bounding Sphere）、
 * 相交检测（如与视锥体或光源的交集）以及物体在变换过程中的包围更新。
 *
 * 主要功能包括：
 * - 球体位置和大小的表示（center + radius）
 * - 应用变换矩阵调整位置和半径（applyMatrix4）
 * - 拷贝其他球体数据（copy）
 *
 * @par 示例用法：
 * @code
 * auto sphere = ff::Sphere::create(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
 * glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
 * sphere->applyMatrix4(transform);  // 半径扩大为 2.0
 * @endcode
 *
 * @note 在 `applyMatrix4` 中，半径只会受到缩放变换的影响，旋转和平移不影响半径大小。
 * @note 本类不提供球体相交检测或包含检测逻辑，需由外部结构（如 Frustum）使用。
 *
 * @author qiang.guo
 * @date 2025-06-19
 */


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