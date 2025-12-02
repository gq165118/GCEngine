#include "renderableObject.h"

namespace ff 
{
	RenderableObject::RenderableObject(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept
	{
		m_geometry = geometry;
		m_material = material;

		m_isRenderableObject = true;
	}

	RenderableObject::~RenderableObject() noexcept { }

	void RenderableObject::onBeforeRender(Renderer* renderer, Scene* scene, Camera* camera)
	{
		if (m_onBeforeRenderCallback)
		{
			m_onBeforeRenderCallback(renderer, scene, camera);
		}
	}
}