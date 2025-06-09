/**
 * @class OrthographicCamera
 * @brief 正交投影摄像机类，用于生成无透视变形的投影矩阵。
 *
 * OrthographicCamera 继承自 Camera，用于在场景中构建正交投影视图，
 * 这意味着物体的远近不会影响其屏幕上的大小。它广泛应用于：
 * - UI 渲染
 * - 2D 游戏
 * - 工具视图（如场景编辑器）
 *
 * 使用 left/right/top/bottom/near/far 参数定义一个正交视锥体（Ortho Frustum），
 * 投影矩阵将在此范围内构造并更新。
 *
 * 构造后可通过 `updateProjectionMatrix()` 更新投影矩阵，或直接使用 `getProjectionMatrix()` 获取。
 *
 * @code
 * auto cam = OrthographicCamera::create(-1, 1, -1, 1, 0.1f, 100.0f);
 * glm::mat4 proj = cam->getProjectionMatrix();
 * @endcode
 *
 * @see ff::Camera, ff::PerspectiveCamera
 * @author qiang.guo
 * @date 2025-06-08
 */


#pragma once
#include "camera.h"

namespace ff 
{

	class OrthographicCamera :public Camera 
	{
	public:
		using Ptr = std::shared_ptr<OrthographicCamera>;
		static Ptr create(float left, float right, float botton, float top, float near, float far)
		{
			return std::make_shared<OrthographicCamera>(left,right,botton,top,near,far);
		}

		OrthographicCamera(float left, float right, float botton, float top, float near, float far) noexcept;

		~OrthographicCamera() noexcept;

	private:
		glm::mat4 updateProjectionMatrix() noexcept override;

	private:
		float m_left{ 0.0f };
		float m_right{ 0.0f };
		float m_top{ 0.0f };
		float m_bottom{ 0.0f };
		float m_near{ 0.0f };
		float m_far{ 0.0f };
	};
}