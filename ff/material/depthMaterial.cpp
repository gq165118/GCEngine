#include "depthMaterial.h"

namespace ff {

	DepthMaterial::DepthMaterial(uint32_t packing) noexcept {
		m_type = MaterialName::DepthMaterial;
		m_isCubeMaterial = true;
		m_drawMode = DrawMode::Triangles;
		m_packing = packing;
	}

	DepthMaterial::~DepthMaterial() noexcept {}
}