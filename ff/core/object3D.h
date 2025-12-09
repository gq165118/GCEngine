/**
 * @class Object3D
 * @brief Object3D是所有空间变换、节点结构等的最基础的类型.
 *
 * Example usage:
 * ```
 * ```
 * @note Thread-safe.
 * @author qiang.guo
 * @date 2025-06-04
 */

#include "../global/base.h"
#pragma once


namespace ff
{
	class ObjectTypeChecker
	{
	public:
		bool m_isRenderableObject{ false };
		bool m_isMesh{ false };
		bool m_isSkinnedMesh{ false };
		bool m_isBone{ false };
		bool m_isScene{ false };
		bool m_isCamera{ false };
		bool m_isPerpectiveCamera{ false };
		bool m_isOrthographicCamera{ false };
		bool m_isGroup{ false };
		bool m_isLight{ false };
		bool m_isAmbientLight{ false };
		bool m_isDirectionalLight{ false };
	};


	class Object3D :public std::enable_shared_from_this<Object3D>, public ObjectTypeChecker
	{
	public:
		using Ptr = std::shared_ptr<Object3D>;
		static Ptr create()
		{
			return std::make_shared<Object3D>();
		}

		Object3D() noexcept;
		
		~Object3D() noexcept;

		//满足动态类型转换
		virtual void fakeFunction() noexcept {};

		void setPosition(float x, float y, float z) noexcept;

		void setPosition(const glm::vec3& position) noexcept;

		void setQuaternion(float x, float y, float z, float w) noexcept;

		void setScale(float x, float y, float z) noexcept;

		void rotateX(float angle) noexcept;

		void rotateY(float angle) noexcept;
		
		void rotateZ(float angle) noexcept;

		//再上一次的基础上旋转
		void rotateAroundAxis(const glm::vec3& axis, float angle) noexcept;

		////将旋转清空，重新旋转
		void setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept;
		
		void lookat(const glm::vec3& target, const glm::vec3& up) noexcept;

		void setLocalMatrix(const glm::mat4& localMatrix) noexcept;

		void setWorldMatrix(const glm::mat4& worldMatrix) noexcept;

		void addChild(const Object3D::Ptr& child) noexcept;

		virtual void updateMatrix() noexcept;

		virtual glm::mat4 updateWorldMatrix(bool updateParent = false, bool updateChildren = false) noexcept;

		glm::mat4 updateModelViewMatrix(const glm::mat4& viewMatrix)noexcept;

		glm::mat3 updateNormalMatrix() noexcept;

		glm::vec3 getPosition() const noexcept;

		glm::vec3 getWorldPosition() const noexcept;

		glm::vec3 getLocalDirection() const noexcept;

		glm::vec3 getWorldDirection() const noexcept;

		glm::vec3 getUp() const noexcept;

		glm::vec3 getRight() const noexcept;

		glm::mat4 getLocalMatrix() noexcept;

		glm::mat4 getWorldMatrix() noexcept;

		glm::mat4 getModelViewMatrix() noexcept;

		glm::mat3 getNormalMatrix() noexcept;
		
		const std::vector<Object3D::Ptr>& getChildren() const noexcept;

		ID getID() const noexcept;

	protected:
		void decompose() noexcept;
	
	public:
		bool m_visible{ true };   //是否进行渲染

		bool m_castShadow{ true };  //是否产生阴影

		std::string m_name;	//obj的名字

		bool m_needUpdateMatrix{ true };  //是否强制对矩阵进行更新

	protected:
		ID m_ID{ 0 };   //全局唯一id

		glm::vec3 m_position{ glm::vec3(0.0f) };

		glm::quat m_quaternion{glm::quat(1.0f, 0.0f, 0.0f, 0.0f)};  //object的旋转变化都采用四元数

		glm::vec3 m_scale{ glm::vec3(1.0) };

		glm::mat4 m_localMatrix = glm::mat4(1.0f);   //对模型坐标进行变换

		glm::mat4 m_worldMatrix = glm::mat4(1.0f);	//将模型顶点从模型坐标系转换到世界坐标系

		//节点系统
		std::weak_ptr<Object3D> m_parent;  //防止循环引用

		std::vector<Object3D::Ptr> m_children{};  //父节点存储了子节点的sharedPtr，建立一次引用,保证子节点的引用计数至少大于1

		glm::mat4 m_modelViewMatrix = glm::mat4(1.0f);  //将模型顶点从模型坐标系转换到当前摄像机坐标系 viewMatrix * worldMatrix

		glm::mat3 m_normalMatrix = glm::mat3(1.0f);	//将模型的normal从模型坐标系转换到摄像机坐标系

	};

}