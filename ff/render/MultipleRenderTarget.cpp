#include "MultipleRenderTarget.h"

namespace ff {

	MultipleRenderTarget::MultipleRenderTarget(const uint32_t& width, const uint32_t& height, const uint32_t& count, const RenderTarget::Options& options) noexcept :
		RenderTarget(width, height, options) {
		m_isMultiRenderTarget = true;

		//mTexture是其父类RenderTarget的哪个ColorAttachment
		//这个mTexture就作为了生成这多个ColorAttachments的模板
		for (uint32_t i = 0; i < count; ++i) {
			mTextures.push_back(m_colorAttachment->clone());
		}
	}

	//其父类已经调用了EventDispatcher
	MultipleRenderTarget::~MultipleRenderTarget() noexcept {}

	void MultipleRenderTarget::pushColor(const Texture::Ptr& texture) noexcept {
		mTextures.push_back(texture);
	}

	void MultipleRenderTarget::setSize(const uint32_t& width, const uint32_t& height) noexcept {
		if (m_width != width || m_height != height) {
			m_width = width;
			m_height = height;

			for (auto& texture : mTextures) {
				texture->m_width = width;
				texture->m_height = height;
			}

			dispose();
		}
	}
}