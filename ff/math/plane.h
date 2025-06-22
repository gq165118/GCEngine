
/**
 * @class Plane
 * @brief 表示三维空间中的一个数学平面，具有法线向量和常数项。
 *
 * `Plane` 提供对空间平面的封装，包括法线（normal）和常数（constant），可用于计算点到平面的距离、
 * 构建几何约束、碰撞检测等常见 3D 应用。
 *
 * 平面的一般方程形式为：
 * ```
 * Ax + By + Cz + D = 0
 * ```
 * 其中 (A, B, C) 为法向量，D 为常数项，对应 `m_normal` 与 `m_constant`。
 *
 * 主要功能包括：
 * - 设置平面系数（setComponents）
 * - 计算点到平面的有符号距离（distanceToPoint）
 *
 * @par 示例用法：
 * @code
 * glm::vec3 normal(0, 1, 0); // 水平向上
 * float d = -5.0f;           // z = 5 的平面
 * auto plane = ff::Plane::create(normal, d);
 *
 * glm::vec3 point(0, 10, 0);
 * float distance = plane->distanceToPoint(point); // 正值表示点在法线一侧
 * @endcode
 *
 * @note 本类不涉及图形绘制，仅作为几何计算使用。
 * @author
 * @date 2025-06-17
 */


#pragma once
#include "../global/base.h"

namespace ff {

	class Plane {
	public:
		using Ptr = std::shared_ptr<Plane>;
		static Ptr create(const glm::vec3& normal, float constant)
		{
			return std::make_shared<Plane>(normal, constant);
		}

		Plane(const glm::vec3& normal, float constant) noexcept
		{
			m_normal = glm::normalize(normal);
			m_constant = constant;
		}

		~Plane() noexcept {};

		//求一个点到平面的距离（与法线通向者距离为正，与法线反向者距离为负）
		float distanceToPoint(const glm::vec3& point) noexcept
		{
			return glm::dot(m_normal, point) + m_constant;
		}

		void setComponents(float x, float y, float z, float w) noexcept
		{
			//平面方程：Ax+By+Cz+D=0，对应法向量是 (A, B, C)
			m_normal.x = x;
			m_normal.y = y;
			m_normal.z = z;

			auto length = glm::length(m_normal);
			m_normal /= length;
			m_constant = w / length;
		}

	public:
		glm::vec3 m_normal = glm::vec3(0.0f);
		float m_constant{ 0.0f };
	};
}