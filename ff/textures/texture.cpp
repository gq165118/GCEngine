#include "texture.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff {

	//Texture::Texture() {}
	Texture::Texture(
		const uint32_t& width,
		const uint32_t& height,
		const DataType& dataType,
		const TextureWrapping& wrapS,
		const TextureWrapping& wrapT,
		const TextureWrapping& wrapR,
		const TextureFilter& magFilter,
		const TextureFilter& minFilter,
		const TextureFormat& format
	) noexcept 
	{
		m_id = Identity::generateID();
		m_width = width;
		m_height = height;
		m_dataType = dataType;
		m_wrapS = wrapS;
		m_wrapR = wrapR;
		m_wrapT = wrapT;
		m_magFilter = magFilter;
		m_minFilter = minFilter;
		m_format = format;
		m_textureType = TextureType::Texture2D;
	}

	Texture::~Texture() noexcept 
	{
		//消亡的时候，通过dispatcher向外发出本texture消亡的消息
		EventBase::Ptr e = EventBase::create("textureDispose");
		e->m_target = this;
		EventDispatcher::getInstance()->dispatchEvent(e);

		if (m_source)
		{
			EventBase::Ptr e = EventBase::create("sourceRelease");
			e->m_target = this;
			EventDispatcher::getInstance()->dispatchEvent(e);
		}
	}

	Texture::Ptr Texture::clone() noexcept 
	{
		auto texture = Texture::create(m_width, m_height, m_dataType, m_wrapS, m_wrapT, m_wrapR, m_magFilter, m_minFilter, m_format);
		texture->m_source = m_source;
		texture->m_usage = m_usage;
		texture->m_textureType = m_textureType;
		texture->m_internalFormat = m_internalFormat;

		return texture;
	}

}