/**
 * @class Frustum
 * @brief 表示视锥体（Frustum）裁剪区域，可用于可见性检测，如包围球相交测试。
 *
 * 视锥体通常用于图形渲染中的裁剪测试（Frustum Culling），判断一个对象是否在相机可见区域内。
 * 本类通过六个平面（左右上下近平面远平面）来定义视锥体，并提供与球体相交测试接口。
 *
 * 主要功能包括：
 * - 从投影矩阵提取视锥体六个平面
 * - 判断一个球体是否与视锥体相交
 *
 * @par 示例用法：
 * @code
 * glm::mat4 projView = projection * view;
 * auto frustum = std::make_shared<ff::Frustum>();
 * frustum->setFromProjectionMatrix(projView);
 * if (frustum->intersectSphere(mesh->getBoundingSphere())) {
 *     // 可见，加入渲染队列
 * }
 * @endcode
 *
 * @note 使用 `glm::value_ptr` 直接访问矩阵数组，需要确保矩阵为列主序（GLM 默认）。
 * @note 本类目前只提供 Sphere 相交测试，如需支持 AABB、OBB 等需扩展。
 * @author qiang.guo
 * @date 2025-06-17
 */


#pragma once 
#include "../global/base.h"
#include "plane.h"
#include "sphere.h"
#include "../objects/renderableObject.h"

namespace ff {

	class Frustum {
	public:
		using Ptr = std::shared_ptr<Frustum>;
		static Ptr create()
		{
			return std::make_shared<Frustum>();
		}

		Frustum() noexcept
		{
			m_toolSphere = Sphere::create(glm::vec3(0.0f), 0.0f);
			for (uint32_t i = 0; i < 6; ++i)
			{
				m_planes.push_back(Plane::create(glm::vec3(0.0f), 0.0f));
			}
		}

		void setFromProjectionMatrix(glm::mat4& matrix)
		{
			auto m = glm::value_ptr(matrix);
			m_planes[0]->setComponents(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
			m_planes[1]->setComponents(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
			m_planes[2]->setComponents(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
			m_planes[3]->setComponents(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
			m_planes[4]->setComponents(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
			m_planes[5]->setComponents(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
		}

		bool intersectObject(const RenderableObject::Ptr& object) noexcept
		{
			auto geomtry = object->getGeometry();

			if (geomtry->getBoundingSphere() == nullptr)
			{
				geomtry->computeBoundingSphere();
			}

			//因为使用的是智能指针，如果直接直接变换会导致geometry里面包含的球不停的累加变换
			m_toolSphere->copy(geomtry->getBoundingSphere());
			m_toolSphere->applyMatrix4(object->getWorldMatrix());

			return intersectSphere(m_toolSphere);
		}

		bool intersectSphere(const Sphere::Ptr& sphere) noexcept
		{
			auto center = sphere->m_center;
			auto radius = sphere->m_radius;

			for (uint32_t i = 0; i < 6; i++)
			{
				//1 计算包围球的球心到当前平面的距离
				auto distance = m_planes[i]->distanceToPoint(center);

				//2 如果球心在平面的正面，那distance一定是正数不进if
				if (distance < -radius)
				{
					return false;
				}
			}
			return true;
		}
		

	private:
		std::vector<Plane::Ptr> m_planes{};
		Sphere::Ptr m_toolSphere{ nullptr };
	};
}