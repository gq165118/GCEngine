#include "renderTarget.h"
#include "../global/eventDispatcher.h"
#include "../tools/identity.h"

namespace ff 
{
	RenderTarget::RenderTarget(const uint32_t& width, const uint32_t& height, const Options& options) noexcept
	{
		m_id = Identity::generateID();

		m_width = width;
		m_height = height;

		m_colorAttachment = Texture::create(
			width,
			height,
			options.mDataType,
			options.mWrapR,
			options.mWrapT,
			options.mWrapR,
			options.mMagFilter,
			options.mMinFilter,
			options.mFormat);

		//本texture是作为渲染输出目标，source为nullptr
		m_colorAttachment->m_usage = TextureUsage::RenderTargetTexture;
		m_colorAttachment->m_internalFormat = options.mInternalFormat;

		m_isNeedDepth = options.mNeedsDepthBuffer;
		m_isNeedStencil = options.mNeedsStencilBuffer;

		m_isRenderTarget = true;
	}
	RenderTarget::~RenderTarget() noexcept
	{
		dispose();
	}

	void RenderTarget::setTexture(const Texture::Ptr& tex) noexcept
	{
		tex->m_width = m_width;
		tex->m_height = m_height;

		m_colorAttachment = tex;
	}

	void RenderTarget::setSize(const uint32_t& width, const uint32_t& height) noexcept
	{
		if (m_width != width || m_height != height)
		{
			m_width = width;
			m_height = height;

			m_colorAttachment->m_width = width;
			m_colorAttachment->m_height = height;

			dispose();
		}
	}

	void RenderTarget::dispose() noexcept
	{
		EventBase::Ptr e = EventBase::create("renderTargetDispose");
		e->m_target = this;
		EventDispatcher::getInstance()->dispatchEvent(e);
	}		
}