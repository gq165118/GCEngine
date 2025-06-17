/**
 * @class GameCameraControl
 * @brief 面向游戏场景的摄像机控制器，支持 WASD 键盘移动和鼠标右键旋转视角。
 *
 * GameCameraControl 是一款典型的第一人称/第三人称摄像机控制器，适用于大多数游戏开发场景。
 * 它基于 `CameraControl` 基类实现，支持以下操作：
 * - 使用键盘 WASD 控制前后左右移动；
 * - 鼠标右键按住并拖动，进行俯仰（Pitch）和偏航（Yaw）旋转；
 * - 支持自定义移动速度与鼠标灵敏度；
 * - 内部记录了当前摄像机朝向、鼠标状态以及旋转角度。
 *
 * 示例用法：
 * @code
 * auto camera = std::make_shared<PerspectiveCamera>(0.1f, 100.f, 1.6f, 60.0f);
 * auto control = GameCameraControl::create(camera);
 * control->setSpeed(0.2f);
 * control->setSensitivity(0.05f);
 * control->onKeyboard(...);
 * control->onMouseMove(...);
 * control->update();
 * @endcode
 *
 * @note 鼠标控制为右键点击激活，释放后自动失效。可用于交互场景中快速观察视角。
 *
 * @see ff::CameraControl, ff::Camera
 * @author qiang.guo
 * @date 2025-06-10
 */


#pragma once 
#include "cameraControl.h"
#include "../global/constant.h"

namespace ff {

	class GameCameraControl :public CameraControl  {
	public:
		static constexpr int MoveFront = 0;
		static constexpr int MoveBack = 1;
		static constexpr int MoveLeft = 2;
		static constexpr int MoveRight = 3;

		using Ptr = std::shared_ptr<GameCameraControl>;
		static Ptr create(const Camera::Ptr& camera)
		{
			return std::make_shared<GameCameraControl>(camera);
		}

		GameCameraControl(const Camera::Ptr& camera)  noexcept;
		~GameCameraControl() noexcept;

		void setSpeed(float speed) noexcept;

		void setSensitivity(float s) noexcept;

		void onKeyboard(const KeyBoardState& action) noexcept override;

		void onMouseAction(const MouseAction& action) noexcept override;

		void onMouseMove(double xpos, double ypos) noexcept override;

		void update() noexcept;

	private:
		float		m_speed{ 0.1f }; //移动速度
		float		m_sensitivity{ 0.1f }; //灵敏度，鼠标移动造成多大程度的旋转
		
		glm::vec3	m_front = glm::vec3(0.0f); //当前相机的朝向
		float		m_pitchAngle{ 0.0f };
		float		m_yawAngle{ -90.f };

		glm::vec2	m_currentMousePosition = glm::vec2(0.0f);
		glm::bvec4	m_moveSate = glm::bvec4(false);  //记录当前正在移动的方向，可以同时多个

		bool		m_rotationState = false; //是否正在旋转,鼠标右键点击后，可以拖动上下左右旋转视角，抬起后就禁用
		bool		m_mouseStateReset = true; 
	};
}