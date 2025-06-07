#pragma once
#include "../global/base.h"
#include "../core/attribute.h"

namespace ff {

	class Box3 {
	public:
		using Ptr = std::shared_ptr<Box3>;
		static Ptr create()
		{
			return std::make_shared<Box3>();
		}

		Box3() noexcept;
		~Box3() noexcept;

		//传入一个Mesh的positionAttribute，给出其包围盒
		void setFormAttribute(const Attribute::Ptr attribute) noexcept
		{
			for (uint32_t i = 0; i < attribute->getCount(); ++i)
			{
				float x = attribute->getX(i);
				float y = attribute->getY(i);
				float z = attribute->getZ(i);

				if (x < m_min.x) { m_min.x = x; }
				if (y < m_min.y) { m_min.y = y; }
				if (z < m_min.z) { m_min.z = z; }

				if (x > m_max.x) { m_max.x = x; }
				if (y > m_max.y) { m_max.y = y; }
				if (z > m_max.z) { m_max.z = z; }
			}

			bool isEmpty() noexcept
			{
				return (m_max.x < m_min.x || m_max.y < m_min.y || m_max.z < m_min.z);
			}

			glm::vec3 getCenter() noexcept
			{
				if (isEmpty())
				{
					return glm::vec3(0.0f);
				}

				//max与min处于立方体的对角线的两个顶点位置
				return (m_max + m_min) / 2.0f;
			}

		}

	public:
		//min是包围盒xyz最小的那个点，max是包围盒xyz最大的那个点
		glm::vec3 m_min = glm::vec3(std::numeric_limits<float>::infinity());  //用float最大数值进行初始化
		glm::vec3 m_max = glm::vec3(-std::numeric_limits<float>::infinity());

	};
}