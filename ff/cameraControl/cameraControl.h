/**
 * @class CameraControl
 * @brief 摄像机控制器基类，用于响应输入事件并更新 Camera 的矩阵。
 *
 * CameraControl 负责将来自键盘和鼠标的输入事件（如 WASD 键移动、鼠标右键拖拽等）
 * 转换为摄像机的位置或方向变换。该类作为基类，支持不同的控制策略派生：
 * - GameCameraControl：类 FPS/第三人称的游戏控制风格（WASD + 鼠标右键旋转）
 * - TrackballControl：适用于 CAD/CAE 等软件的轨迹球交互方式
 *
 * 它内部持有一个 `Camera::Ptr`，可对任意派生自 `Camera` 的实例进行控制，
 * 包括 `PerspectiveCamera` 和 `OrthographicCamera`。
 *
 * 接口说明：
 * - `onKeyboard`：响应键盘输入，如移动方向
 * - `onMouseAction`：响应鼠标点击、拖拽、滚轮等
 * - `onMouseMove`：响应鼠标移动事件（x/y 位置变化）
 *
 * 使用示例：
 * @code
 * @endcode
 *
 * @see ff::Camera, ff::PerspectiveCamera, ff::OrthographicCamera
 * @author qiang.guo
 * @date 2025-06-09
 */


#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../camera/camera.h"

namespace ff {

	//任务：根据键盘鼠标，更改camera的矩阵
	//种类：gameCameraControl wasd 鼠标右键 trackBallControl cad/cae软件当中
	class CameraControl {
	public:
		using Ptr = std::shared_ptr<CameraControl>;
		static Ptr create(const Camera::Ptr& camera)
		{
			return std::make_shared<CameraControl>(camera);
		}

		CameraControl(const Camera::Ptr& camera) noexcept;
		~CameraControl() noexcept;

		//bit集合 108键
		virtual void onKeyboard(const KeyBoardState& action) noexcept;

		virtual void onMouseAction(const MouseAction& action) noexcept;

		virtual void onMouseMove(double xpos, double ypos) noexcept;

	protected:
		Camera::Ptr m_camera{ nullptr }; //正交相机，透视相机
	};
}