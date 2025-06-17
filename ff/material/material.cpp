#include "material.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {

	Material::Material() noexcept 
	{
		m_id = Identity::generateID();
		m_type = MaterialName::Material;
		m_isMaterial = true;
	}

	Material::~Material() noexcept 
	{
		//再析构的时候，向外发出消息，通知监听函数们，哪一个材质消亡了
		EventBase::Ptr e = EventBase::create("materialDispose");
		e->m_target = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}

}