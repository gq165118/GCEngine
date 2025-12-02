/**
 * @class RenderableObject
 * @brief 可渲染的3D对象，绑定几何体与材质，作为渲染引擎的基本绘制单元。
 *
 * `RenderableObject` 继承自 `Object3D`，是场景图中可以被渲染的核心实体。
 * 它绑定了一份几何数据（`Geometry`）和一份材质（`Material`），并提供了一个在渲染前可执行的用户回调。
 *
 * 典型用途：
 * - 渲染静态网格对象，如模型、地面、墙体等；
 * - 搭配不同材质实例，用于实现 PBR、Phong、纯色等多种效果；
 * - 支持用户设置自定义的 `onBeforeRender` 回调，用于在 GPU 提交前动态修改状态；
 *
 * @par 示例
 * @code
 * auto geometry = ff::Geometry::create();
 * auto material = ff::MeshPhongMaterial::create();
 * auto mesh = ff::RenderableObject::create(geometry, material);
 * mesh->m_onBeforRenderCallback = [](Renderer* renderer, Scene* scene, Camera* camera) {
 *     // 可自定义 Uniform 设置或状态更改
 * };
 * @endcode
 *
 * @see ff::Geometry, ff::Material, ff::Renderer, ff::Scene, ff::Camera
 * @author
 * @date 2025-06-12
 */


#pragma once
#include "../core/object3D.h"
#include "../core/geometry.h"
#include "../material/material.h"
#include "../global/base.h"
#include "../global/constant.h"

namespace ff 
{
	class Renderer;
	class Scene;
	class Camera;

	class RenderableObject : public Object3D
	{
	public:
		using OnBeforRenderCallback = std::function<void(Renderer*, Scene*, Camera*)>;

		using Ptr = std::shared_ptr<RenderableObject>;
		static Ptr create(const Geometry::Ptr& geometry, const Material::Ptr& material)
		{
			return std::make_shared<RenderableObject>(geometry, material);
		}

		RenderableObject(const Geometry::Ptr& geometry, const Material::Ptr& material) noexcept;

		~RenderableObject() noexcept;

		auto getGeometry() const noexcept { return m_geometry; }
		
		auto getMaterial() const noexcept { return m_material; }

		//在本物体渲染前会调用本函数，允许用户指定渲染前做哪些处理
		void onBeforeRender(Renderer* renderer, Scene* scene, Camera* camera);

	public:
		OnBeforRenderCallback m_onBeforeRenderCallback{ nullptr }; //在本物体可绘制物体进行渲染之前，会回调这个函数进行通知

	protected:
		Geometry::Ptr m_geometry{ nullptr };
		Material::Ptr m_material{ nullptr };

	};
	
}