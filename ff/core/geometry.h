/**
 * @class Geometry
 * @brief Geometry 表示 Mesh 的基础几何数据容器，封装了所有顶点属性与索引数据，并支持包围盒/球计算。
 *
 * Geometry 是对一个 3D 网格对象顶点数据的组织抽象，内部维护一个属性字典（AttributeMap），支持存储各种顶点属性：
 * - 位置 Position
 * - 法线 Normal
 * - 颜色 Color
 * - UV 坐标
 * - 切线/副切线 Tangent / Bitangent
 *
 * 同时，它也维护一个索引属性（Index），用于绘制时进行顶点复用。每个 Geometry 实例具有唯一 ID。
 *
 * 支持自动计算包围盒（BoundingBox）和包围球（BoundingSphere），常用于碰撞检测或视锥裁剪等场景。
 *
 * Example usage:
 * ```cpp

 * ```
 * @note 本类通过智能指针 `std::shared_ptr` 管理自身生命周期，支持 `shared_from_this()`。
 * @note 所有属性数据均以 Attribute 形式存储，并可通过名称进行访问、设置和删除。
 * @note 非线程安全，使用时应注意数据并发访问。
 *
 * @see ff::Attributef, ff::Attributei, ff::Box3, ff::Sphere
 *
 * @author qiang.guo
 * @date 2025-06-08
 */



#pragma once
#include "../global/base.h"
#include "attribute.h"
#include "../math/sphere.h"
#include "../math/box3.h"

namespace ff
{
	class Geometry : public std::enable_shared_from_this<Geometry>
	{
	public:
		using AttributeMap = std::unordered_map<std::string, Attributef::Ptr>;
		
		using Ptr = std::shared_ptr<Geometry>;
		static Ptr create()
		{
			return std::make_shared<Geometry>();
		}

		Geometry() noexcept;
		~Geometry() noexcept;

		void setAttribute(const std::string& name, Attributef::Ptr attribute) noexcept;

		Attributef::Ptr getAttribute(const std::string& name) noexcept;

		void setIndex(const Attributei::Ptr& index) noexcept;

		void deleteAttribute(const std::string& name) noexcept;

		bool hasAttribute(const std::string& name) noexcept;

		const AttributeMap& getAttribute() const noexcept;

		ID getID() const noexcept;

		auto getIndex() const noexcept { return m_indexAttribute; }

		void computeBonudingBox() noexcept;

		void computeBoundingSphere() noexcept;

		Sphere::Ptr getBoundingSphere() const noexcept { return m_bonudingSphere; }

		Box3::Ptr getBoundingBox() const noexcept { return m_boundingBox; }

	protected:
		ID m_id{ 0 }; 
		AttributeMap m_attributes{}; //按照名称-值的方式村饭了所有本mesh的Attribute
		Attributei::Ptr m_indexAttribute{ nullptr };  //index的Attribute单独存放

		Box3::Ptr	m_boundingBox{ nullptr };	//包围盒
		Sphere::Ptr m_boundingSphere{ nullptr };  //包围球

	};


}