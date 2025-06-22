#include "mesh.h"

namespace ff 
{
	Mesh::Mesh(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept
		:RenderableObject(geometry, material)
	{
		m_isMesh = true;
	}

	Mesh::~Mesh() noexcept { }
}