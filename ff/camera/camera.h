/**
 * @class Camera
 * @brief Camera 表示一个虚拟摄像机，用于从视角生成视图矩阵和投影矩阵。
 *
 * Camera 继承自 Object3D，具备世界空间中的位置、旋转、缩放等变换能力，
 * 是 3D 场景中核心的观察者对象。它既可以作为主摄像机，也可以用于生成光照视角（例如用于阴影贴图）。
 *
 * 相机的主要职责是生成如下两类矩阵：
 * - 世界矩阵的逆矩阵 (m_workMatrixInverse)：用于将世界坐标变换到相机视角。
 * - 投影矩阵 (m_projectionMatrix)：用于将相机视角坐标投影到裁剪空间（屏幕）。
 *
 * 此类为抽象基类，必须实现 `updateProjectionMatrix()` 以生成特定的投影方式（例如透视或正交）。
 *
 * @note 每帧调用 `updateWorldMatrix()` 可以同步更新逆矩阵。
 * @note 投影矩阵应在视锥参数改变（如fov, near, far）时更新。
 *
 * 示例使用：
 * ```cpp
 * ```
 * @see ff::Object3D, ff::PerspectiveCamera, ff::OrthographicCamera
 * @author qiang.guo
 * @date 2025-06-08
 */

#pragma once
#include "../global/base.h"
#include "../core/object3D.h"

namespace ff {

	//worldMatrix:将顶点从模型坐标系，转换到世界坐标系
	//viewMatrix：将顶点从世界坐标系，转换到模型坐标系
	//逆矩阵（Inverse Matrix）A与B互为逆矩阵，那么A * B = I（单位阵）
	class Camera : public Object3D
	{
	public:
		using Ptr = std::shared_ptr<Camera>;

		Camera() noexcept;

		~Camera() noexcept;

		glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildre = false) noexcept override;

		glm::mat4 getWorldMatrixInverse() noexcept { return m_workMatrixInverse; }

		glm::mat4 getProjectionMatrix() noexcept { return m_projectionMatrix; }

		virtual glm::mat4 updateProjectionMatrix() noexcept = 0; 

	protected:
		glm::mat4 m_workMatrixInverse = glm::mat4(1.0f);
		glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
	};

	
}