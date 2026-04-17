#include "cubeMaterial.h"

namespace ff {

	CubeMaterial::CubeMaterial() noexcept {
		m_type = MaterialName::CubeMaterial;
		m_isCubeMaterial = true;
		m_drawMode = DrawMode::Triangles;
	}

	CubeMaterial::~CubeMaterial() noexcept {}
}