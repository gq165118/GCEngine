/**
 * @class DriverBindingState
 * @brief 表示与一个 Geometry 对应的 OpenGL VAO 状态封装。
 *
 * 一个 Geometry 对象（包含顶点属性、索引等）对应一个 VAO（Vertex Array Object），
 * DriverBindingState 封装了该 VAO 以及其绑定的 Attribute 映射。
 * 其主要用于缓存 VAO 的绑定状态，避免重复生成与绑定。
 *
 * @note 通常不直接使用本类，而由 DriverBindingStates 统一管理其生命周期。
 * @see DriverBindingStates, DriverAttributes
 * @author qiang.guo
 * @date 2025-06-20
 */

 /**
  * @class DriverBindingStates
  * @brief 管理所有 Geometry 与其对应的 VAO 状态（DriverBindingState）的关系。
  *
  * 每一个 Geometry 会被映射到唯一一个 DriverBindingState 实例，
  * 并绑定其相关 Attribute（位置、颜色、法线等）到 VAO 中。
  * 本类提供以下功能：
  * - 判断 VAO 是否需要更新（needsUpdate）
  * - 创建并缓存新的 VAO 状态（createBindingState, saveCache）
  * - 将 Attribute 绑定到 VAO 中（setupVertexAttributes）
  * - 控制 VAO 的生成、绑定和释放（createVAO, bindVAO, releaseStatesOfGeometry）
  *
  * Example usage:
  * @code
  * auto bindingStates = DriverBindingStates::create(driverAttributes);
  * auto bindingState = bindingStates->getBindingState(geometry);
  * if (bindingStates->needsUpdate(geometry, indexAttr)) {
  *     bindingStates->setup(geometry, indexAttr);
  * }
  * @endcode
  *
  * @note 本类负责缓存 VAO 绑定状态以减少 CPU 到 GPU 的冗余数据传输。
  * @note 对于每帧中重复使用的几何体，可通过内部缓存复用 VAO 提高效率。
  *
  * @see DriverAttributes, Geometry, Attribute, DriverBindingState
  * @author qiang.guo
  * @date 2025-06-20
  */


#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "../../core/geometry.h"
#include "../../core/object3D.h"
#include "../../core/attribute.h"
#include "../../material/material.h"
#include "driverAttributes.h"
//#include "driverPrograms.h"

namespace ff 
{
	class DriverBindingStates;
	class DriverBindingState 
	{
		friend DriverBindingStates;
	public:
		using Ptr = std::shared_ptr<DriverBindingState>;
		static Ptr create() 
		{
			return std::make_shared <DriverBindingState>();
		}

		DriverBindingState() noexcept;

		~DriverBindingState() noexcept;

	private:
		GLuint m_vao{ 0 };
		
		//cache校验相关
		std::unordered_map<std::string, ID> m_attributes{};  //key:attribute的名字 value:attribute的id作为
		ID m_indexID{ 0 }; //记录对应的geometry的indexAttribute的id
		uint32_t m_attributeNum{ 0 };  //记录总共有多少个attribute

	};

	//一个VAO与一个Geometry一一对应
	class DriverBindingStates {
	public:
		//key:geometry的ID号  value：BindingState这里面蕴含着一个VAO
		using GeometryKeyMap = std::unordered_map<ID, DriverBindingState::Ptr>;

		using Ptr = std::shared_ptr<DriverBindingStates>;

		static Ptr create(const DriverAttributes::Ptr& attributes)
		{
			return std::make_shared<DriverBindingStates>(attributes);
		}

		DriverBindingStates(const DriverAttributes::Ptr& attributes);
		
		~DriverBindingStates();

		DriverBindingState::Ptr getBindingState(const Geometry::Ptr& geometry) noexcept;

		void setup(const Geometry::Ptr& geometry, const Attributei::Ptr& index);

		DriverBindingState::Ptr createBindingState(GLuint vao) noexcept;

		bool needsUpdate(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept;

		void saveCache(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept;

		void setupVertexAttributes(const Geometry::Ptr& geometry) noexcept;

		GLuint createVAO() noexcept;

		void bindVAO(GLuint vao) noexcept;

		void releaseStatesOfGeometry(ID geometryID) noexcept;

	private:
		DriverAttributes::Ptr	m_attributes{ nullptr }; //所有的vbo
		DriverBindingState::Ptr m_currentBindingState{ nullptr }; //当前帧vao
		GeometryKeyMap			m_bindingStates{};  //存储geo 与 vao的对应关系

	};
}