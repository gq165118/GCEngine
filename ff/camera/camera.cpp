#include "camera.h"

namespace ff {

	Camera::Camera() noexcept {
		m_isCamera = true;
	}

	Camera::~Camera() noexcept {}

	glm::mat4 Camera::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {
		Object3D::updateWorldMatrix(updateParent, updateChildren);
		m_workMatrixInverse = glm::inverse(m_worldMatrix);

		return m_worldMatrix;
	}
}