/**
 * @class DriverObjects
 * @brief 管理所有渲染对象（RenderableObject）的几何数据更新与缓存。
 *
 * 本类是渲染器中一个关键的桥梁组件，用于从高层的 RenderableObject（如 Mesh）
 * 中提取并管理其绑定的 Geometry 几何数据。它调用并依赖底层的：
 * - DriverGeometries：更新几何体及其顶点属性到 GPU（如 VBO / VAO）；
 * - DriverAttributes：管理属性上传和更新；
 * - DriverInfo：用于记录统计信息，例如当前处理的几何数量。
 *
 * 它通过一个 `m_updateMap` 缓存来避免对同一 Object 多次重复处理，
 * 确保每帧只更新一次 Geometry 状态，从而优化性能。
 *
 * @note 本类不会直接渲染对象，仅用于几何体数据准备和缓存管理。
 * @see ff::RenderableObject
 * @see ff::DriverGeometries
 * @see ff::DriverAttributes
 * @see ff::DriverInfo
 *
 * @author qiang.guo
 * @date 2025-06-23
 */

#pragma once
#include "../../global/base.h"
#include "../../objects/renderableObject.h"
#include "driverGeometries.h"
#include "driverAttributes.h"
#include "driverInfo.h"

namespace ff 
{
	class DriverObjects 
	{
	public:
		using Ptr = std::shared_ptr<DriverObjects>;
		static Ptr create(
			const DriverGeometries::Ptr& geometries,
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info)
		{
			return std::make_shared<DriverObjects>(geometries, attributes, info);
		}

		DriverObjects(
			const DriverGeometries::Ptr& geometries,
			const DriverAttributes::Ptr& attributes,
			const DriverInfo::Ptr& info
		) noexcept;
		
		~DriverObjects() noexcept;

		Geometry::Ptr update(const RenderableObject::Ptr& object) noexcept;

	private:
		std::unordered_map<ID, uint32_t> m_updateMap{};

		DriverInfo::Ptr m_info{ nullptr };
		DriverAttributes::Ptr m_attributes{ nullptr };
		DriverGeometries::Ptr m_geometries{ nullptr };
	};
}