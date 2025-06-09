#include "perspectiveCamera.h"

namespace ff {

	PerspectiveCamera::PerspectiveCamera(float near, float far, float aspect, float fov) noexcept 
	{
		m_isPerpectiveCamera = true;
		m_near = near;
		m_far = far;
		m_aspect = aspect;
		m_fov = fov;

		updateProjectionMatrix();
	}

	PerspectiveCamera::~PerspectiveCamera() noexcept {}

	glm::mat4 PerspectiveCamera::updateProjectionMatrix() noexcept 
	{
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
		return m_projectionMatrix;
	}
}