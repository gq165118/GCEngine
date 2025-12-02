#include "driverGeometries.h"
#include "../../global/eventDispatcher.h"

namespace ff
{

	DriverGeometries::DriverGeometries(
		const DriverAttributes::Ptr& attributes,
		const DriverInfo::Ptr& info,
		const DriverBindingStates::Ptr& bindingStates
	) noexcept
	{
		m_attributes = attributes;
		m_info = info;
		m_bindingStates = bindingStates;

		EventDispatcher::getInstance()->addEventListener("geometryDispose", this, &DriverGeometries::onGeometryDispose);
	}

	DriverGeometries::~DriverGeometries()
	{
		EventDispatcher::getInstance()->removeEventListener("geometryDispose", this, &DriverGeometries::onGeometryDispose);
	}

	//给了一个机会， 可以在每一次update的之前，对geometry相关数据做一次更新
	Geometry::Ptr DriverGeometries::get(const Geometry::Ptr geometry) noexcept
	{
		auto iter = m_geometries.find(geometry->getID());
		if (iter != m_geometries.end())
		{
			if (iter->second == true)
			{
				return geometry;
			}
		}

		m_geometries[geometry->getID()] = true;

		m_info->m_memery.m_geometries++;

		return geometry;
	}

	void DriverGeometries::onGeometryDispose(const EventBase::Ptr& e) noexcept
	{
		const auto geometry = static_cast<Geometry*>(e->mTarget);

		m_geometries.erase(geometry->getID());

		m_info->m_memery.m_geometries--;
	}

	void DriverGeometries::update(const Geometry::Ptr& geometry) noexcept
	{
		const auto geometryAttributes = geometry->getAttributes();
		
		for (const auto& iter : geometryAttributes)
		{
			m_attributes->update(iter.second, BufferType::ArrayBuffer);
		}
	}


}