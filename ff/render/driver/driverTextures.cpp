#include "driverTextures.h"
#include "../MultipleRenderTarget.h"

namespace ff 
{
	DriverTexture::DriverTexture() noexcept {
	}

	DriverTexture::~DriverTexture() noexcept {
		dispose();
	}

	void DriverTexture::dispose() noexcept 
	{ 
		if (m_handle)
		{
			glDeleteFramebuffers(GL_TEXTURE_2D, &m_handle);
			m_handle = 0;
		}
	}

	DriverTextures::DriverTextures(const DriverInfo::Ptr& info, const DriverRenderTargets::Ptr& renderTargets) noexcept 
	{
		m_info = info;
		m_renderTargets = renderTargets;

		EventDispatcher::getInstance()->addEventListener("textureDispose", this, &DriverTextures::onTextureDestroy);
	}

	DriverTextures::~DriverTextures() noexcept 
	{
		EventDispatcher::getInstance()->removeEventListener("textureDispose", this, &DriverTextures::onTextureDestroy);
	}

	void DriverTextures::update(const Texture::Ptr& texture) noexcept 
	{
		auto dTexture = get(texture);

		//mNeedsUpdate在texture初次创建的时候，会是true
		//在使用者更改了texture相关的东西之后，可以手动将其置为true，就会触发更改
		if (texture->m_needUpdate)
		{
			texture->m_needUpdate = false;
			setupDriverTexture(texture);
		}

	}

	DriverTexture::Ptr DriverTextures::setupDriverTexture(const Texture::Ptr& texture) noexcept 
	{
		DriverTexture::Ptr dTexture = get(texture);
		texture->m_needUpdate = false;

		if (!dTexture->m_handle)
		{
			glGenTextures(1, &dTexture->m_handle);
		}

		glBindTexture(toGL(texture->m_textureType), dTexture->m_handle);

		//设置纹理参数
		glTexParameteri(toGL(texture->m_textureType), GL_TEXTURE_MIN_FILTER, toGL(texture->m_minFilter));
		glTexParameteri(toGL(texture->m_textureType), GL_TEXTURE_MAG_FILTER, toGL(texture->m_magFilter));
		glTexParameteri(toGL(texture->m_textureType), GL_TEXTURE_WRAP_S, toGL(texture->m_wrapS));
		glTexParameteri(toGL(texture->m_textureType), GL_TEXTURE_WRAP_T, toGL(texture->m_wrapT));
		glTexParameteri(toGL(texture->m_textureType), GL_TEXTURE_WRAP_R, toGL(texture->m_wrapR));

		if (texture->m_textureType == TextureType::Texture2D)
		{
			//必须是贴图专用得texture而且不是渲染目标，才能有土坯那数据
			const byte* data = (texture->getUsage() == TextureUsage::SamplerTexture) ? texture->m_source->m_data.data() : nullptr;

			//1 开辟内存空间
			//2 传输图片数据
			glTexImage2D(GL_TEXTURE_2D, 0, toGL(texture->m_internalFormat), texture->m_width, texture->m_height, 0, toGL(texture->m_format), toGL(texture->m_dataType), data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else  //TextureCubeMap
		{

		}

		glBindTexture(toGL(texture->m_textureType), 0);
		m_info->m_memery.m_textures++;

		return dTexture;
	}

	void DriverTextures::setupFBOColorAttachment(const GLuint& fbo, const GLenum& attachment, const Texture::Ptr& texture) noexcept 
	{
		auto dTexture = get(texture);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, toGL(texture->m_textureType), dTexture->m_handle,0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DriverTextures::setupFBODepthStencilAttachment(const RenderTarget::Ptr& renderTarget) noexcept 
	{
		auto dRenderTarget = m_renderTargets->get(renderTarget);
		if (!dRenderTarget->m_frameBuffer)
		{
			std::cout << "Error: frameBuffer has not been geterated, when setupFBODepthStecilAttachment" << std::endl;
			return;
		}
		  
		//TODO: support stencil buffer
		//如果需要DepthTest，并且用户指定了一张用户创建的Texture

		if (renderTarget->m_isNeedDepth && renderTarget->m_depthAttachment)
		{
			setupDepthTexture(dRenderTarget->m_frameBuffer, renderTarget);
		}
		else if (renderTarget->m_isNeedDepth)
		{
			setupDepthRenderBuffer(dRenderTarget->m_frameBuffer, renderTarget);
		}

	}

	void DriverTextures::setupDepthTexture(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget) noexcept 
	{
		if (!renderTarget->m_depthAttachment || !renderTarget->m_isNeedDepth)
		{
			std::cout << "Error: renderTarget has no depth info when setupDepthTexture";
			return;
		}

		auto depthTexture = renderTarget->m_depthAttachment;
		auto dDepthTexture = get(depthTexture);
		setupDriverTexture(depthTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, toGL(depthTexture->m_textureType), dDepthTexture->m_handle, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DriverTextures::setupDepthRenderBuffer(const GLuint& frameBuffer, const RenderTarget::Ptr& renderTarget) noexcept
	{
		auto dRenderTarget = m_renderTargets->get(renderTarget);

		//创建RenderBuffer
		glGenRenderbuffers(1, &dRenderTarget->m_depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, dRenderTarget->m_depthRenderBuffer);

		//为renderBuffer对象开辟空间
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, renderTarget->m_width, renderTarget->m_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);


		//向frameBuffer进行绑定
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dRenderTarget->m_depthRenderBuffer);

		GLint error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	DriverTexture::Ptr DriverTextures::get(const Texture::Ptr& texture) noexcept 
	{
		auto iter = m_textures.find(texture->getID());
		if (iter != m_textures.end())
		{
			iter = m_textures.insert(std::make_pair(texture->getID(), DriverTexture::create())).first;
		}

		return iter->second;
	}

	void DriverTextures::bindTexture(const Texture::Ptr& texture, GLenum textureUnit) 
	{
		//GL_TEXTURE0  GL_TEXTURE1 GL_TEXTURE2....
		//GL_TEXTURE1 = GL_TEXTURE0+1
		//GL_TEXTURE2 = GL_TEXTURE0+2
		
	}

	void DriverTextures::setupRenderTarget(const RenderTarget::Ptr& renderTarget) noexcept 
	{
		
	}

	void DriverTextures::onTextureDestroy(const EventBase::Ptr& e) noexcept 
	{
		
	}
}