/**
 * @class Scene
 * @brief Scene 是 3D 场景图的根节点，负责管理渲染对象的容器，背景环境及材质覆盖设置。
 *
 * Scene 继承自 Object3D，作为所有 3D 对象（如 Mesh、Light、Camera 等）的父节点存在。
 * 场景不仅提供层次化管理的能力，还支持全局渲染控制，例如：
 *
 * - m_overrideMaterial：当设置时，强制所有渲染物体使用该材质，忽略自身材质，常用于调试或特殊渲染。
 * - m_background：设置场景背景纹理，通常是立方体纹理（CubeTexture）用于实现天空盒效果。
 *
 * 使用方式：
 * @code
 * auto scene = ff::Scene::create();
 * scene->m_background = myCubeTexture;
 * scene->m_overrideMaterial = myDebugMaterial;
 * scene->add(mesh); // 添加 Object3D 派生对象
 * @endcode
 *
 * @see ff::Object3D, ff::Material, ff::CubeTexture
 * @author
 * @date 2025-06-13
 */


#pragma once
#include "../global/base.h"
#include "../core/object3D.h"
#include "../material/material.h"
#include "../textures/cubeTexture.h"

namespace ff 
{
	class Scene : public Object3D
	{
	public:
		using Ptr = std::shared_ptr<Scene>;
		static Ptr create()
		{
			return std::make_shared <Scene>();
		}

		Scene() noexcept;
		~Scene() noexcept;

		//指定整个场景里面所有物体使用哪个material，不管各个物体是否拥有自己的material,强制使用整个material
		Material::Ptr m_overrideMaterial = nullptr;

		CubeTexture::Ptr m_background = nullptr;  //天空盒

	};
	
}