#include "gameCameraControl.h"

namespace ff {

	GameCameraControl::GameCameraControl(const Camera::Ptr& camera) noexcept :CameraControl(camera) {}

	GameCameraControl::~GameCameraControl() noexcept {}

	void GameCameraControl::setSpeed(float speed) noexcept 
	{
		m_speed = speed;
	}

	void GameCameraControl::setSensitivity(float s) noexcept 
	{
		m_sensitivity = s;
	}

	void GameCameraControl::onKeyboard(const KeyBoardState& action) noexcept 
	{
		if (action[WKey])
			m_moveSate[MoveFront] = true;
		else
			m_moveSate[MoveFront] = false;
		
		if (action[AKey])
			m_moveSate[MoveLeft] = true;
		else
			m_moveSate[MoveLeft] = false;

		if (action[SKey])
			m_moveSate[MoveBack] = true;
		else
			m_moveSate[MoveBack] = false;

		if (action[DKey])
			m_moveSate[MoveRight] = true;
		else
			m_moveSate[MoveRight] = false;
	}

	void GameCameraControl::onMouseAction(const MouseAction& action) noexcept 
	{
		if (action == MouseAction::RightDown)
		{
			m_rotationState = true;
		}
		else if (action == MouseAction::RightUp)
		{
			m_rotationState = false;
			//标定是否第一次按下鼠标
			m_mouseStateReset = true;
		}
		
	}

	void GameCameraControl::onMouseMove(double xpos, double ypos) noexcept 
	{
		//如果右键没有按下则不处理
		if (!m_rotationState)	return;

		if (m_mouseStateReset)
		{
			m_mouseStateReset = false;
		}
		else
		{
			float deltaX = xpos - m_currentMousePosition.x;
			float deltaY = m_currentMousePosition.y - ypos; //win窗口原点在左上角，y轴向下

			float angleX = deltaX * m_sensitivity;
			float angleY = deltaY * m_sensitivity;

			m_pitchAngle += angleY;
			m_yawAngle += angleX;

			//不能仰面翻过去，也不能前滚翻
			if (m_pitchAngle >= 90.f)
				m_pitchAngle = 89.0f;

			if (m_pitchAngle <= -90.0f) 
				m_pitchAngle = -89.0f;
			
			m_front.y = sin(glm::radians(m_pitchAngle));
			m_front.x = cos(glm::radians(m_yawAngle)) * cos(glm::radians(m_pitchAngle));
			m_front.z = sin(glm::radians(m_yawAngle)) * cos(glm::radians(m_pitchAngle));

			//注意传进去的是看向哪个点，而不是直接把方向传进去
			m_camera->lookat(m_front + m_camera->getPosition(), glm::vec3(0.0, 1.0, 0.0));
		}

		m_currentMousePosition.x = xpos;
		m_currentMousePosition.y = ypos;

	}

	//在onMouseMove的时候，已经计算完毕了camera的朝向， 即旋转矩阵，但是还没有更新位置
	void GameCameraControl::update() noexcept
	{
		//当前camera这个object的模型坐标系，-z方向
		glm::vec3 front = m_camera->getLocalDirection();

		//camera的右边（模型坐标系）
		glm::vec3 right = m_camera->getRight();

		glm::vec3 position = m_camera->getPosition();

		glm::vec3 direction = glm::vec3(0.0f);

		bool needUpdate = false;

		if (m_moveSate[MoveFront])
		{
			direction += front;
			needUpdate = true;
		}

		if (m_moveSate[MoveFront])
		{
			direction += -front;
			needUpdate = true;
		}

		if (m_moveSate[MoveLeft])
		{
			direction += -right;
			needUpdate = true;
		}

		if (m_moveSate[MoveRight])
		{
			direction += right;
			needUpdate = true;
		}

		if (needUpdate && (direction.x != 0.0 || direction.y != 0.0 || direction.z != 0.0))
		{
			direction = glm::normalize(direction);
			position += direction * m_speed;
			
			m_camera->setPosition(position);
		}
	}

}