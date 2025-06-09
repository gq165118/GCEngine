/**
 * @class PerspectiveCamera
 * @brief 透视投影摄像机类，用于模拟真实视觉中的透视效果。
 *
 * PerspectiveCamera 是 ff::Camera 的派生类，用于根据视野角度、长宽比、
 * 近平面和远平面构建典型的透视投影矩阵。它适用于大多数 3D 游戏和应用中，
 * 实现物体随距离缩放的自然透视现象。
 *
 * 投影矩阵由 updateProjectionMatrix() 构建，内部使用 glm::perspective 实现。
 *
 * 参数说明：
 * - `near`：近平面距离（必须 > 0）
 * - `far`：远平面距离
 * - `aspect`：视口宽高比（width / height）
 * - `fov`：视野角度（Field of View），单位为度（degree）
 *
 * @code
 * auto camera = std::make_shared<PerspectiveCamera>(0.1f, 100.0f, 16.0f/9.0f, 60.0f);
 * glm::mat4 proj = camera->getProjectionMatrix();
 * @endcode
 *
 * @see ff::Camera, ff::OrthographicCamera
 * @author qiang.guo
 * @date 2025-06-08
 */


#include "camera.h"

namespace ff 
{

	class PerspectiveCamera : public Camera 
	{
	public:
		using Ptr = std::shared_ptr<PerspectiveCamera>;

		PerspectiveCamera(float near, float far, float aspect, float fov) noexcept;

		~PerspectiveCamera() noexcept;

	private:
		glm::mat4 updateProjectionMatrix() noexcept override;


	private:
		float m_far{ 100.0f };
		float m_near{ 0.1f };
		float m_aspect{ 1.3f };
		float m_fov{ 45.0f };

	};
}