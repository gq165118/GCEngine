#include "meshBasicMaterial.h"

namespace ff
{
	MeshBasicMaterial::MeshBasicMaterial() noexcept
	{
		m_type = MaterialName::MeshBasicMaterial;
		m_isMeshBasicMaterial = true;
		m_drawMode = DrawMode::Triangles;
	}

	MeshBasicMaterial::~MeshBasicMaterial() noexcept
	{

	}
}