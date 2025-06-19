#include "driverRenderList.h"

namespace ff {

	RenderItem::RenderItem() noexcept {}

	RenderItem::~RenderItem() noexcept {}

	DriverRenderList::DriverRenderList() {}

	DriverRenderList::~DriverRenderList() {}

	//每一帧开始的时候，渲染列表都会被清空
	void DriverRenderList::init() noexcept 
	{
		m_renderItemIndex = 0;
		m_opaqueue.clear();
		m_transparents.clear();
	}

	//在这里我们push进去一个可渲染物体的相关参数，解包的方式
	//为什么需要解包传送,有可能会有替代,举例：本来object拥有一个material，但是scene也拥有一个overrideMaterial
	//那么就不能使用object原来的material
	void DriverRenderList::push(
		const RenderableObject::Ptr& object,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material,
		const uint32_t& groupOrder,
		float z
	) noexcept
	{
		//每一帧都会重新构建renderList，所以比如有5个物体，如果不做renderItem的缓存，那么
		//每一帧都要重新new 5个renderItem
		//所以上一帧假设我们已经生成了10个renderItem，那么会将其缓存在renderItemCache里面，在需要的时候，就
		//从中取出一个给到renderList使用
		const auto renderItem = getNextRenderItem(object, geometry, material, groupOrder, z);

		if (material->m_transparent)
		{
			m_transparents.push_back(renderItem);
		}
		else
		{
			m_opaqueue.push_back(renderItem);
		}
	}

	RenderItem::Ptr DriverRenderList::getNextRenderItem(
		const RenderableObject::Ptr& object,
		const Geometry::Ptr& geometry,
		const Material::Ptr& material,
		const uint32_t& groupOrder,
		float z
	) noexcept 
	{
		RenderItem::Ptr renderItem = nullptr;
		if (m_renderItemIndex > m_renderItemCache.size()) //当前renderItem的总数已经大于了缓存数量，那么就重新生成
		{
			renderItem = RenderItem::create();
			m_renderItemCache.push_back(renderItem);
		}
		else  //否则直接从缓存当中抽欺第mRenderItemIndex个item
		{
			renderItem = m_renderItemCache[m_renderItemIndex];
		}

		renderItem->m_id = object->getID();
		renderItem->m_object = object;
		renderItem->m_geometry = geometry;
		renderItem->m_material = material;
		renderItem->m_z = z;

		m_renderItemIndex++;  

		return renderItem;
	}

	void DriverRenderList::sort(
		const RenderListSortFunction& opaqueSort,
		const RenderListSortFunction& transparentSort) noexcept 
	{
		if (!m_opaqueue.empty()) std::sort(m_opaqueue.begin(), m_opaqueue.end(), opaqueSort);

		if (!m_transparents.empty()) std::sort(m_transparents.begin(), m_transparents.end(), transparentSort);
	}

	//如果缓存当中存在10个item，本帧只有5个物体需要渲染，那么就会使用到10个item里面的五个
	//但是！剩下的没有使用到的五个，item里面，却存有object、materil、geometry的智能指针,则其内存无法被释放
	//所以finish需要检测剩下的没有使用到的item，然后依次将其智能指针的引用置空
	void DriverRenderList::finish() noexcept 
	{
		auto listSize = m_renderItemCache.size();

		for (uint32_t i = m_renderItemIndex; i < listSize; ++i)
		{
			auto renderItem = m_renderItemCache[i];
			if (renderItem == nullptr)
			{
				break;
			}

			renderItem->m_id = 0;
			renderItem->m_object = nullptr;
			renderItem->m_geometry = nullptr;
			renderItem->m_groupOrder = 0;
			renderItem->m_material = nullptr;
			renderItem->m_z = 0;
		}
	}
}