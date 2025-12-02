#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../textures/depthTexture.h"

namespace ff 
{
	class RTTypeChecker
	{
	public:
		bool m_isRenderTarget = false;
		bool m_isMultiRenderTarget = false;
	};

	class DriverTextures;
	class DriverRenderTargets;

	class RenderTarget : public RTTypeChecker
	{
	public:
		friend DriverTextures;
		friend DriverRenderTargets;
		
		struct Options
		{
			TextureWrapping	mWrapS{ TextureWrapping::RepeatWrapping };
			TextureWrapping	mWrapT{ TextureWrapping::RepeatWrapping };
			TextureWrapping	mWrapR{ TextureWrapping::RepeatWrapping };
			TextureFilter	mMagFilter{ TextureFilter::LinearFilter };
			TextureFilter	mMinFilter{ TextureFilter::LinearFilter };
			TextureFormat	mFormat{ TextureFormat::RGBA };
			DataType		mDataType{ DataType::UnsignedByteType };
			TextureFormat	mInternalFormat{ TextureFormat::RGBA };

			bool			mNeedsDepthBuffer{ true };
			bool			mNeedsStencilBuffer{ false };

			//允许外部设定
			DepthTexture::Ptr	m_depthTexture{ nullptr };
		};

	public:
		using Ptr = std::shared_ptr<RenderTarget>;
		static Ptr create(const uint32_t& width, const uint32_t& height, const Options& options)
		{
			return std::make_shared<RenderTarget>(width, height, options);
		}

		RenderTarget(const uint32_t& width, const uint32_t& height, const Options& options) noexcept;
		~RenderTarget() noexcept;
	
		void setTexture(const Texture::Ptr& tex) noexcept;

		auto getTexture() const noexcept { return m_colorAttachment; }

		virtual void setSize(const uint32_t& width, const uint32_t& height) noexcept;

		void dispose() noexcept;

	protected:
		ID				m_id{ 0 };
		uint32_t		m_width{ 0 };
		uint32_t		m_height{ 0 };

		Texture::Ptr	m_colorAttachment{ nullptr };
		Texture::Ptr	m_depthAttachment{ nullptr };

		bool			m_isNeedDepth{ true };
		bool			m_isNeedStencil{ false };

	};


	
}