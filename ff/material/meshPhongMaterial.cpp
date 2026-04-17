#include "meshPhongMaterial.h"

namespace ff {

	MeshPhongMaterial::MeshPhongMaterial() noexcept {
		m_type = MaterialName::MeshPhongMaterial;
		m_isMeshPhongMaterial = true;
		m_drawMode = DrawMode::Triangles;
	}

	MeshPhongMaterial::~MeshPhongMaterial() noexcept {}
}