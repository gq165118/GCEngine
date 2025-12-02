#include "geometry.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff
{
	Geometry::Geometry() noexcept
	{
		m_id = Identity::generateID();
	}

	Geometry::~Geometry() noexcept
	{
		//对所有的监听函数，发出本Geometry消亡的消息
		EventBase::Ptr e = EventBase::create("geometryDispose");
		e->mTarget = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}

	void Geometry::setAttribute(const std::string& name, Attributef::Ptr attribute) noexcept
	{
		m_attributes[name] = attribute;
	}

	Attributef::Ptr Geometry::getAttribute(const std::string& name) noexcept
	{
		auto iter = m_attributes.find(name);
		if (iter != m_attributes.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void Geometry::setIndex(const Attributei::Ptr& index) noexcept
	{
		m_indexAttribute = index;
	}

	void Geometry::deleteAttribute(const std::string& name) noexcept
	{
		auto iter = m_attributes.find(name);
		if (iter != m_attributes.end())
		{
			m_attributes.erase(iter);
		}
	}

	bool Geometry::hasAttribute(const std::string& name) noexcept
	{
		auto iter = m_attributes.find(name);
		if (iter != m_attributes.end())
		{
			return true;
		}
		return false;
	}

	const Geometry::AttributeMap& Geometry::getAttributes() const noexcept
	{
		return m_attributes;
	}

	ID Geometry::getID() const noexcept
	{
		return m_id;
	}

	void  Geometry::computeBonudingBox() noexcept
	{
		auto position = getAttribute("position");

		if (position == nullptr)
		{
			std::cout << "Error: geometry has no position when computeBoundingBox" << std::endl;
			return;
		}

		if (m_boundingBox == nullptr)
		{
			m_boundingBox = Box3::create();
		}

		m_boundingBox->setFormAttribute(position);
	}

	void  Geometry::computeBoundingSphere() noexcept
	{
		computeBonudingBox();
		if (m_boundingSphere == nullptr)
		{
			m_boundingSphere = Sphere::create(glm::vec3(0.0f), 0.0f);
		}

		//包围球跟包围盒共享一个center
		m_boundingSphere->m_center = m_boundingBox->getCenter();

		auto position = getAttribute("position");
		if (position == nullptr)
		{
			return;
		}

		//找到距离当前球心最大距离的点
		float maxRadiusSq = 0;
		for (uint32_t i = 0; i < position->getCount(); ++i)
		{
			glm::vec3 point = glm::vec3(position->getX(i), position->getY(i), position->getZ(i));

			//计算point到center的距离
			glm::vec3 radiusVector = m_boundingSphere->m_center - point;

			// 性能考虑直接记录其平方
			maxRadiusSq = std::max(glm::dot(radiusVector, radiusVector), maxRadiusSq);
		}
		
		//开方求取radius
		m_boundingSphere->m_radius = std::sqrt(maxRadiusSq);
	}

}