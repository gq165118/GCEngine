#include "object3D.h"
#include "../tools/identity.h"

namespace ff
{
	Object3D::Object3D() noexcept
	{
		m_ID = Identity::generateID();
	}

	Object3D::~Object3D() noexcept
	{

	}

	void Object3D::setPosition(float x, float y, float z) noexcept
	{
		setPosition(glm::vec3(x, y, z));
	}

	void Object3D::setPosition(const glm::vec3& position) noexcept
	{
		m_localMatrix[3].x = position.x;  //glm列优先存储
		m_localMatrix[3].y = position.y;
		m_localMatrix[3].z = position.z;

		m_position = position;
	}

	//将四元数反映到本地矩阵上，拆解成对应参数
	void Object3D::setQuaternion(float x, float y, float z, float w) noexcept
	{
		glm::quat quaternion(w, x, y, z);

		//可能已经经过缩放
		float scaleX = glm::length(glm::vec3(m_localMatrix[0]));
		float scaleY = glm::length(glm::vec3(m_localMatrix[1]));
		float scaleZ = glm::length(glm::vec3(m_localMatrix[2]));

		//将四元数转换为旋转矩阵
		glm::mat4 rotateMatrix = glm::mat4_cast(quaternion);

		m_localMatrix[0] = rotateMatrix[0] * scaleX;
		m_localMatrix[1] = rotateMatrix[1] * scaleY;
		m_localMatrix[2] = rotateMatrix[2] * scaleZ;

		decompose();
	}

	void Object3D::setScale(float x, float y, float z) noexcept
	{
		//1 通过normalize 去掉之前的scale影响，再进行当前的scale
		auto col0 = glm::normalize(glm::vec3(m_localMatrix[0])) * x;
		auto col1 = glm::normalize(glm::vec3(m_localMatrix[1])) * y;
		auto col2 = glm::normalize(glm::vec3(m_localMatrix[2])) * z;

		//2 重新设置本地矩阵
		m_localMatrix[0] = glm::vec4(col0, 0.0f);
		m_localMatrix[1] = glm::vec4(col1, 0.0f);
		m_localMatrix[2] = glm::vec4(col2, 0.0f);

		decompose();
	}

	void Object3D::rotateX(float angle) noexcept
	{
		//1 先获取到当前模型状态下的右侧方向
		glm::vec3 rorateAxis = glm::vec3(m_localMatrix[0]);

		//2 针对这个右侧方向作为旋转轴来进行旋转
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle),rorateAxis);
		m_localMatrix = rotateMatrix * m_localMatrix;

		decompose();
	}

	void Object3D::rotateY(float angle) noexcept
	{
		glm::vec3 rorateAxis = glm::vec3(m_localMatrix[1]);

		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), rorateAxis);
		m_localMatrix = rotateMatrix * m_localMatrix;

		decompose();
	}

	void Object3D::rotateZ(float angle) noexcept
	{
		glm::vec3 rorateAxis = glm::vec3(m_localMatrix[2]);

		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), rorateAxis);
		m_localMatrix = rotateMatrix * m_localMatrix;

		decompose();
	}

	void Object3D::rotateAroundAxis(const glm::vec3& axis, float angle) noexcept
	{
		m_localMatrix = glm::rotate(m_localMatrix, glm::radians(angle), axis);

		decompose();
	}

	void Object3D::setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept
	{
		//1 获取旋转矩阵
		glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);

		//2 保留缩放
		float scaleX = glm::length(glm::vec3(m_localMatrix[0]));
		float scaleY = glm::length(glm::vec3(m_localMatrix[1]));
		float scaleZ = glm::length(glm::vec3(m_localMatrix[2]));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));

		//3 重新计算本地矩阵  (列相等)
		m_localMatrix[0] = rotateMatrix[0];
		m_localMatrix[1] = rotateMatrix[1];
		m_localMatrix[2] = rotateMatrix[2];

		m_localMatrix *= scaleMatrix;  //RS

		decompose();
	}

	void Object3D::lookat(const glm::vec3& target, const glm::vec3& up) noexcept
	{
		//1 拆解
		float scaleX = glm::length(glm::vec3(m_localMatrix[0]));
		float scaleY = glm::length(glm::vec3(m_localMatrix[1]));
		float scaleZ = glm::length(glm::vec3(m_localMatrix[2]));

		glm::vec3 position = glm::vec3(m_localMatrix[3]);

		//2 构建局部坐标系
		auto nTarget = glm::normalize(target - position) * scaleZ;
		auto nRight = glm::normalize(glm::cross(up, -nTarget)) * scaleX;
		auto nUp = glm::normalize(glm::cross(nRight, nTarget)) * scaleY;

		//3 组装本地矩阵
		m_localMatrix[0] = glm::vec4(nRight, 0.0f);
		m_localMatrix[1] = glm::vec4(nUp, 0.0f);
		m_localMatrix[2] = glm::vec4(-nTarget, 0.0f);
		m_localMatrix[3] = glm::vec4(position, 1.0f);

		decompose();
	}

	void Object3D::setLocalMatrix(const glm::mat4& localMatrix) noexcept
	{
		m_localMatrix = localMatrix;

		decompose();
	}

	void Object3D::setWorldMatrix(const glm::mat4& worldMatrix) noexcept
	{
		m_worldMatrix = worldMatrix;
	}

	void Object3D::addChild(const Object3D::Ptr& child) noexcept
	{
		//1 确认加入的子节点并不是自己
		if (child == shared_from_this())
		{
			return;
		}

		//2 添加的child是一个自身的智能指针，child是用WeakPtr来接纳的，避免了循环引用
		child->m_parent = shared_from_this();

		//3 查找当前节点是否已经加入本child
		auto iter = std::find(m_children.begin(), m_children.end(), child);
		if (iter != m_children.end()) return;

		m_children.push_back(child);
		 
	}

	void Object3D::updateMatrix() noexcept
	{
		if (m_needUpdateMatrix)
		{
			m_needUpdateMatrix = false;
			auto translateMatrix = glm::translate(glm::mat4(1.0f), m_position);
			auto rotateMatrix = glm::mat4_cast(m_quaternion);
			auto scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

			m_localMatrix = translateMatrix * rotateMatrix * scaleMatrix;
		}
	}

	//通过层级matrix相乘，得到最后的转换到世界坐标系的矩阵
	glm::mat4 Object3D::updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept
	{
		//1 检查有没有父节点
		if (!m_parent.expired() && updateParent)
		{
			auto parent = m_parent.lock();
			parent->updateWorldMatrix(true, false);
		}

		//2 跟新自己的loaclMatrix并初始化worldMatrix，如果没有父节点，那么二者相等
		updateMatrix();
		m_worldMatrix = m_localMatrix;

		//3 如果有父节点，需要做成父节点的woldMatrix， 从而把上方所有的节点的影响带入
		if (!m_parent.expired())
		{
			auto parent = m_parent.lock();
			m_worldMatrix = parent->m_worldMatrix * m_worldMatrix;
		}

		//4 依次更新子节点的worldMatrix
		if (updateChildren)
		{
			for (auto& child : m_children)
			{
				child->updateWorldMatrix(false, true);
			}
		}

		return m_worldMatrix;
	}

	glm::mat4 Object3D::updateModelViewMatrix(const glm::mat4& viewMatrix)noexcept
	{
		m_modelViewMatrix = viewMatrix * m_worldMatrix;

		return m_modelViewMatrix;
	}

	glm::mat3 Object3D::updateNormalMatrix() noexcept
	{
		//normalMatrix 由于存在scale的影响，不能直接变换，否则normal会无法保证从垂直
		m_normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_modelViewMatrix)));

		return m_normalMatrix;
	}

	glm::vec3 Object3D::getPosition() const noexcept
	{
		return glm::vec3(m_localMatrix[3]);
	}

	glm::vec3 Object3D::getWorldPosition() const noexcept
	{
		return glm::vec3(m_worldMatrix[3]);
	}

	glm::vec3 Object3D::getLocalDirection() const noexcept
	{
		return glm::normalize(-glm::vec3(m_localMatrix[2]));
	}

	glm::vec3 Object3D::getWorldDirection() const noexcept
	{
		return glm::normalize(-glm::vec3(m_worldMatrix[2]));
	}

	glm::vec3 Object3D::getUp() const noexcept
	{
		return glm::normalize(glm::vec3(m_localMatrix[1]));
	}

	glm::vec3 Object3D::getRight() const noexcept
	{
		return glm::normalize(glm::vec3(m_localMatrix[0]));
	}

	glm::mat4 Object3D::getLocalMatrix() noexcept
	{
		return m_localMatrix;
	}

	glm::mat4 Object3D::getWorldMatrix() noexcept
	{
		return m_worldMatrix;
	}

	glm::mat4 Object3D::getModelViewMatrix() noexcept
	{
		return m_modelViewMatrix;
	}

	glm::mat3 Object3D::getNormalMatrix() noexcept
	{
		return m_normalMatrix;
	}

	const std::vector<Object3D::Ptr>& Object3D::getChildren() const noexcept
	{
		return m_children;
	}

	ID Object3D::getID() const noexcept
	{
		return m_ID;
	}

	void Object3D::decompose() noexcept
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		//将m_localMatrix矩阵拆解 本地矩阵、 缩放、 旋转信息（四元数）、平移分量
		glm::decompose(m_localMatrix, m_scale, m_quaternion, m_position, skew, perspective);
	}
}

