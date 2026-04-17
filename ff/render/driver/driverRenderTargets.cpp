#include "driverRenderTargets.h"
#include "../../global/eventDispatcher.h"

namespace ff 
{
	DriverRenderTarget::DriverRenderTarget() noexcept {}

	DriverRenderTarget::~DriverRenderTarget() noexcept 
	{
		if (m_frameBuffer)
		{
			glDeleteFramebuffers(1, &m_frameBuffer);
		}
	}

	void DriverRenderTarget::generateFrameBuffer() noexcept 
	{
		glGenFramebuffers(1, &m_frameBuffer);
	}

	DriverRenderTargets::DriverRenderTargets() noexcept 
	{
		EventDispatcher::getInstance()->addEventListener("renderTargetDispose", this, &DriverRenderTargets::onRenderTargetDispose);
	}

	DriverRenderTargets::~DriverRenderTargets() noexcept 
	{
		EventDispatcher::getInstance()->removeEventListener("renderTargetDispose", this, &DriverRenderTargets::onRenderTargetDispose);
	}

	DriverRenderTarget::Ptr DriverRenderTargets::get(const RenderTarget::Ptr& renderTarget) noexcept
	{
		auto id = renderTarget->m_id;
		auto iter = m_renderTargets.find(id);
		if (iter != m_renderTargets.end())
		{
			//insert 会返回一个 std::pair<iterator, bool>：
			iter = m_renderTargets.insert(std::make_pair(id, DriverRenderTarget::create())).first;
		}

		return iter->second;
	}

	void DriverRenderTargets::onRenderTargetDispose(const EventBase::Ptr& e)
	{
		auto renderTarget = static_cast<RenderTarget*>(e->mTarget);
		auto iter = m_renderTargets.find(renderTarget->m_id);
		if (iter == m_renderTargets.end())
		{
			return;
		}

		m_renderTargets.erase(iter);
	}

}
