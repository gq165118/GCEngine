#include "orthographicCamera.h"

namespace ff 
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far) noexcept 
	{
		m_isOrthographicCamera = true;
		m_left = left;
		m_right = right;
		m_top = top;
		m_bottom = bottom;
		m_near = near;
		m_far = far;

		updateProjectionMatrix();
	}

	OrthographicCamera::~OrthographicCamera() noexcept {}

	glm::mat4 OrthographicCamera::updateProjectionMatrix() noexcept 
	{
		m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
		return m_projectionMatrix;
	}
}