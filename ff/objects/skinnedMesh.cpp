#include "skinnedMesh.h"

namespace ff {

	SkinnedMesh::SkinnedMesh(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept:
		Mesh(geometry, material) {
		m_isSkinnedMesh = true;
	}

	SkinnedMesh::~SkinnedMesh() noexcept {}

	void SkinnedMesh::bind(const Skeleton::Ptr& skeleton) noexcept {
		mSkeleton = skeleton;
	}
}