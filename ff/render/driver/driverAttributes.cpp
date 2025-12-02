#include "driverAttributes.h"

namespace ff
{
	DriverAttribute::DriverAttribute() noexcept
	{

	}

	DriverAttribute::~DriverAttribute() noexcept
	{
		if (m_handle)
		{
			glDeleteBuffers(1, &m_handle);
		}
	}

	DriverAttributes::DriverAttributes() noexcept
	{
		EventDispatcher::getInstance()->addEventListener("attributeDispose", this, &DriverAttributes::onAttributeDispose);
	}

	DriverAttributes::~DriverAttributes() noexcept
	{
		EventDispatcher::getInstance()->removeEventListener("attributeDispose", this, &DriverAttributes::onAttributeDispose);
	}

	void DriverAttributes::remove(ID attributeID) noexcept
	{
		auto iter = m_attributes.find(attributeID);
		if (iter != m_attributes.end())
		{
			m_attributes.erase(iter);
		}
	}

	//对应前端Attribute，Attribute在析构的时候会创建attributeDispose事件
	void DriverAttributes::onAttributeDispose(const EventBase::Ptr& e)
	{
		ID attrID = *((ID*)e->mpUserData);
		remove(attrID);  //将后端的id销毁
	}

}