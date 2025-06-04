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
		bool mIsRenderableObject{ false };
		bool mIsMesh{ false };
		bool mIsSkinnedMesh{ false };
		bool mIsBone{ false };
		bool mIsScene{ false };
		bool mIsCamera{ false };
		bool mIsPerpectiveCamera{ false };
		bool mIsOrthographicCamera{ false };
		bool mIsGroup{ false };
		bool mIsLight{ false };
		bool mIsAmbientLight{ false };
		bool mIsDirectionalLight{ false };
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
		void setRotateAroundAxis(const glm::vec3& axis, float angle) noexcept;

		////将旋转清空，重新旋转
		void rotateAroundAxis(const glm::vec3& axis, float angle) noexcept;

		void lookat(const glm::vec3& target, const glm::vec3& up) noexcept;

		void setLocalMatrix(const glm::mat4& localMatrix) noexcept;

		void setWorldMatrix(const glm::mat4& worldMatrix) noexcept;

		void addChild(const Object3D::Ptr& child) noexcept;

	protected:
		void decompose() noexcept;


	protected:
		ID m_ID{ 0 };   //全局唯一id

		glm::vec3 m_position{ glm::vec3(0.0f) };

		glm::quat m_quaternion{glm::quat(1.0f, 0.0f, 0.0f, 0.0f)};  //object的旋转变化都采用四元数

		glm::vec3 m_scale{ glm::vec3(1.0) };

		glm::mat4 m_localMatrix = glm::mat4(1.0f);   //对模型坐标进行变换

		glm::mat4 m_worldMatrix = glm::mat4(1.0f);	//将模型顶点从模型坐标系转换到世界坐标系

		//节点系统
		std::weak_ptr<Object3D> m_parent;  //防止循环引用

		std::vector<Object3D::Ptr> m_children{};  //存储子节点的sharedPtr,建立一次引用

		glm::mat4 m_modelViewMatrix = glm::mat4(1.0f);  //将模型顶点从模型坐标系转换到当前摄像机坐标系 viewMatrix * worldMatrix

		glm::mat3 m_normalMatrix = glm::mat3(1.0f);	//将模型的normal从模型坐标系转换到摄像机坐标系

	};

}