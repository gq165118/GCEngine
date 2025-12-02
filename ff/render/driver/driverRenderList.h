/**
 * @class RenderItem
 * @brief RenderItem 表示渲染引擎中最小的渲染单元，绑定了几何体、材质与对象。
 *
 * 所有可渲染对象（如 Mesh、Line 等）在渲染阶段都会被解析成 RenderItem 实例，
 * 用于统一排序与渲染处理。每个 RenderItem 包含以下关键属性：
 * - 几何体（Geometry）
 * - 材质（Material）
 * - 所属对象（RenderableObject）
 * - 排序相关属性（m_z、m_groupOrder、m_id）
 *
 * 可配合排序函数（如 `smallerZFirstSort` 和 `biggerZFirstSort`）实现对不透明物体和透明物体的不同渲染顺序。
 *
 * @note RenderItem 不涉及 OpenGL 渲染指令，仅作为渲染调度的数据结构。
 * @see DriverRenderList
 * @author qiang.guo
 * @date 2025-06-19
 */


 /**
  * @class DriverRenderList
  * @brief 管理每一帧的渲染列表，包括不透明和透明物体的分组与排序。
  *
  * DriverRenderList 是 GPU 渲染的关键组织结构，负责：
  * - 存储当前帧的渲染对象（RenderItem）
  * - 区分不透明物体与透明物体（根据 Material 的 transparent 标志）
  * - 提供排序策略支持（如 Z 轴排序、GroupOrder 等）
  * - 使用缓存机制避免频繁创建 RenderItem
  *
  * Example usage:
  * @code
  * renderList->init();
  * renderList->push(meshObj, meshGeo, meshMat, groupOrder, cameraZ);
  * renderList->sort(); // 排序非透明与透明物体
  * renderList->finish();
  * auto& opaques = renderList->getOpaqueue();
  * auto& transparents = renderList->getTransparents();
  * @endcode
  *
  * @note 每帧使用流程：init -> push -> sort -> finish。
  * @note 渲染顺序依赖 m_groupOrder, m_z, m_id 进行优先级判断。
  *
  * @see RenderItem, RenderableObject, Material
  * @author qiang.guo
  * @date 2025-06-19
  */


#pragma once
#include "../../global/base.h"
#include "../../objects/renderableObject.h"
#include "../../core/geometry.h"
#include "../../material/material.h"

namespace ff 
{

	//mesh line skinnedMesh 都会被解析为RenderItem
	class RenderItem 
	{
	public:
		using Ptr = std::shared_ptr<RenderItem>;
		static Ptr create()
		{
			return std::make_shared<RenderItem>();
		}
		
		RenderItem() noexcept;
		~RenderItem() noexcept;


	public:
		ID						m_id{ 0 };
		float					m_z{ 0 };  //用来排序-渲染透明物体的时候，从远到进进行渲染
		RenderableObject::Ptr	m_object{ nullptr };
		Material::Ptr			m_material{ nullptr };
		Geometry::Ptr			m_geometry{ nullptr };
		uint32_t				m_groupOrder{ 0 }; //影响渲染顺序
	};

	using RenderListSortFunction = std::function<bool(const RenderItem::Ptr&, const RenderItem::Ptr&)>;

	//排序的原因:
	//1 opaque物体需要排序，
	//>表示大的在前面
	static bool smallerZFirstSort(const RenderItem::Ptr& item0, const RenderItem::Ptr& item1)
	{
		//首先保证groupOrder大的物体先绘制
		if (item0->m_groupOrder != item1->m_groupOrder)
		{
			return item0->m_groupOrder > item1->m_groupOrder;
		}
		else if (item0->m_z != item1->m_z) //小z 排在前面
		{
			return item0->m_z < item1->m_z;
		}
		else
		{
			return item0->m_id > item1->m_id;
		}
	}

	static bool biggerZFirstSort(const RenderItem::Ptr& item0, const RenderItem::Ptr& item1)
	{
		//首先保证groupOrder大的物体先绘制
		if (item0->m_groupOrder != item1->m_groupOrder)
		{
			return item0->m_groupOrder > item1->m_groupOrder;
		}
		else if (item0->m_z != item1->m_z) //小z 排在前面
		{
			return item0->m_z > item1->m_z;
		}
		else
		{
			return item0->m_id > item1->m_id;
		}
	}


	//driverRenderList用来存储基础的渲染单元
	class DriverRenderList 
	{
	public:
		using Ptr = std::shared_ptr<DriverRenderList>;
		static Ptr create()
		{
			return std::make_shared<DriverRenderList>();
		}
		
		DriverRenderList();
		~DriverRenderList();

		void init() noexcept;

		//向渲染队列中加入一个renderItem
		void push(
			const RenderableObject::Ptr& object,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material,
			const uint32_t& groupOrder,
			float z
		) noexcept;

		//排序操作,允许给出对于非透明物体以及透明物体的排序规则函数
		void sort(
			const RenderListSortFunction& opaqueSort = smallerZFirstSort, //为了earlyz
			const RenderListSortFunction& transparentSort = biggerZFirstSort) noexcept;//为了颜色混合正确

		//在每一次构建完毕渲染列表的时候调用finish
		void finish() noexcept;

		const auto& getOpaques() const noexcept { return m_opaqueue; }

		const auto& getTransparents() const noexcept { return m_transparents; }

	private:
		//每一次push一个可渲染物体，都会调用本函数，不管是重新生成renderItem还是
		//从cache里面获取一个可用的，都会返回一个可用的renderItem
		RenderItem::Ptr getNextRenderItem(
			const RenderableObject::Ptr& object,
			const Geometry::Ptr& geometry,
			const Material::Ptr& material,
			const uint32_t& groupOrder,
			float z
		) noexcept;

	private:
		uint32_t m_renderItemIndex{ 0 }; //用来计算当前渲染队列的item数，每一帧开始的时候在init里面都会被置为0
		std::vector<RenderItem::Ptr> m_opaqueue{}; //存储非透明物体
		std::vector<RenderItem::Ptr> m_transparents{};  //存储透明物体
		std::vector<RenderItem::Ptr> m_renderItemCache{};  //缓存renderItem
	};
}