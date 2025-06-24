/**
 * @class DriverGeometries
 * @brief 管理所有用于渲染的 Geometry 对象及其对应的 GPU 状态。
 *
 * 该类主要负责将 Geometry 数据（顶点属性、索引信息等）绑定到 GPU 上的 VBO/VAO，
 * 并维护一个缓存系统，避免重复上传以及冗余统计。
 *
 * 它与 DriverAttributes（VBO）、DriverBindingStates（VAO） 和 DriverInfo（资源统计）
 * 共同协作，为后端渲染器提供完整几何数据的 GPU 映射和生命周期管理。
 *
 * 功能包括：
 * - 根据 Geometry 自动生成所需的 VAO/VBO 并绑定；
 * - 更新变更的属性数据；
 * - 管理 geometry 的释放与统计；
 * - 减少资源冗余、提升缓存效率。
 *
 * @note Geometry 的更新会触发相关 GPU 数据重新同步（如顶点坐标变化）。
 * @note 本类不直接执行绘制，仅准备所需数据。
 *
 * @see ff::DriverAttributes
 * @see ff::DriverBindingStates
 * @see ff::DriverInfo
 *
 * @author qiang.guo
 * @date 2025-06-21
 */


#pragma once
#include "../../global/base.h"
#include "../../core/geometry.h"
#include "driverAttributes.h"
#include "driverInfo.h"
#include "driverBindingState.h"

namespace ff
{
	class DriverGeometries
	{
	public:
		using Ptr = std::shared_ptr<DriverGeometries>;
		static Ptr create(
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info,
			const DriverBindingStates::Ptr& bindingStates)
		{
			return std::make_shared<DriverGeometries>(attributes, info, bindingStates);
		}


		DriverGeometries(
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info,
			const DriverBindingStates::Ptr& bindingStates
		) noexcept;

		~DriverGeometries();

		Geometry::Ptr get(const Geometry::Ptr geometry) noexcept;

		void onGeometryDispose(const EventBase::Ptr& e) noexcept;

		void update(const Geometry::Ptr& geometry) noexcept;

	private:
		DriverAttributes::Ptr m_attributes{ nullptr };   //所有属性vbo的集合
		DriverInfo::Ptr m_info{ nullptr };

		DriverBindingStates::Ptr m_bindingStates{ nullptr };//geo 与vao关系集合 

		std::unordered_map<ID, bool> m_geometries{};  //记录当前这个geometry是否被计算过info一次
	};

}