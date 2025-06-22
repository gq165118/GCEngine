/**
 * @class Mesh
 * @brief Mesh 是场景中常用的网格对象，继承自 RenderableObject，代表具有几何数据和材质的可渲染实体。
 *
 * Mesh 是 3D 场景中最常见的渲染对象之一。它绑定了 Geometry（顶点、索引数据）和 Material（材质属性），
 * 用于生成最终的渲染结果。Mesh 对象可以用于承载各种 3D 模型，并由渲染器提交到 GPU 进行绘制。
 *
 * Mesh 通过静态工厂函数 `create` 构造，并继承了 RenderableObject 的接口，例如
 * `onBeforeRender` 回调，可在渲染前进行一些自定义的状态调整。
 *
 * Example usage:
 * @code
 * auto geometry = ff::Geometry::create();
 * auto material = ff::Material::create(); // 或具体材质类型，如 MeshPhongMaterial 等
 * auto mesh = ff::Mesh::create(geometry, material);
 * // 后续可以设置 mesh 的变换、渲染回调等
 * @endcode
 *
 * @see ff::RenderableObject, ff::Geometry, ff::Material
 * @author
 * @date 2025-06-12
 */

#pragma once
#include "../global/base.h"
#include "renderableObject.h"

namespace ff 
{
	class Mesh : public RenderableObject
	{
	public:
		using Ptr = std::shared_ptr<Mesh>;
		static Ptr create(const Geometry::Ptr& geometry, const Material::Ptr& material)
		{
			return std::make_shared<Mesh>(geometry, material);
		}

		Mesh(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept;
		~Mesh();

	private:

	};
}
