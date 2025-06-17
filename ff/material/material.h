/**
 * @class Material
 * @brief 渲染材质基类，封装了光栅状态、混合状态、深度状态与常见贴图。
 *
 * `ff::Material` 是所有材质类型的通用基类，包含了一套统一的渲染状态设置结构，适用于光栅化渲染。
 * 它支持：
 * - 面剔除与绘制模式（如双面、正面、三角形/线段绘制）；
 * - 混合模式（透明度、Blending 参数）；
 * - 深度检测控制；
 * - 支持基础贴图（如 Diffuse、Normal、Specular、环境贴图 EnvMap）；
 * - 提供版本号和更新标记以支持 GPU 后端的缓存/同步；
 * - 提供类型标志（通过 `MaterialTypeChecker` 派生）以支持 RTTI 替代。
 *
 * 通常不直接使用 `Material`，而是通过派生类如：
 * - `MeshBasicMaterial`：无光照影响的材质
 * - `MeshPhongMaterial`：基于 Blinn-Phong 模型的高光材质
 * - `LineBasicMaterial`：用于线段渲染
 * - `DepthMaterial`：专用于深度绘制场景，如阴影图
 * - `CubeMaterial`：用于环境贴图展示
 *
 * 成员变量按照功能划分为：
 * - 光栅控制区（`m_frontFace`, `m_side`, `m_drawMode`）
 * - 混合控制区（`m_transparent`, `m_blendingType`, `m_blendSrc` 等）
 * - 深度控制区（`m_depthTest`, `m_depthWrite`, `m_depthFunction` 等）
 * - 纹理区（`m_diffuseMap`, `m_normalMap`, `m_specularMap`, `m_envMap`）
 *
 * 示例用法：
 * @code
 * @endcode
 *
 * @see ff::MeshPhongMaterial, ff::MeshBasicMaterial, ff::CubeTexture, ff::Texture
 * @author qiang.guo
 * @date 2025-06-12
 */


#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../textures/texture.h"
#include "../textures/cubeTexture.h"

namespace ff 
{

	class MaterialTypeChecker 
	{
	public:
		bool m_isMaterial = false;
		bool m_isMeshBasicMaterial = false;
		bool m_isMeshPhongMaterial = false;
		bool m_isLineBasicMaterial = false;
		bool m_isCubeMaterial = false;
		bool m_isDepthMaterial = false;
	};

	class Material:public MaterialTypeChecker 
	{
	public:
		using Ptr = std::shared_ptr<Material>;
		static Ptr create()
		{
			return std::make_shared<Material>();
		}

		Material() noexcept;

		~Material() noexcept;

	public:
		std::string getType() const noexcept { return m_type; }
		ID getID() const noexcept { return m_id; }

		bool				m_needUpdate{ true };  //表示在backend里面，是否需要更新材质参数

		uint32_t			m_version{ 1 }; //version 用于首次解析

		//=========== raster 光栅化 ============//
		FrontFace			m_fronetFace{ FrontFace::FrontCounterClockWise };
		Side				m_side{ Side::DoubleSide };

		DrawMode			m_drawMode{ DrawMode::Triangles };

		//=========== blending 混合=============//
		bool				m_transparent{ false }; //控制是否开启blending
		float				m_opacity{ 1.0f }; //透明度，越小越透明
		BlendingType		m_blendingType{ BlendingType::CustomBlending };
		BlendingFactor		m_blendSrc{ BlendingFactor::SrcAlpha };
		BlendingFactor		m_blendDst{ BlendingFactor::OneMinusSrcAlpha };
		BlendingEquation	m_blendEuqation{ BlendingEquation::AddEquation };
		BlendingFactor		m_blendSrcAlpha{ BlendingFactor::None };
		BlendingFactor		m_blendDstAlpha{ BlendingFactor::Zero };
		BlendingEquation	mBlendEquationAlpha{ BlendingEquation::AddEquation };

		//============== depth ================//
		bool				m_depthTest{ true }; //是否开启深度检测
		bool				m_depthWrite{ true }; //是否在深度检测通过后用本物体的深度覆盖原来的深度
		CompareFunction		m_depthFunction{ CompareFunction::LessOrEqual };
		double				m_depthClearColor{ 1.0 };

		//diffuse 
		Texture::Ptr		m_diffuseMap{ nullptr };

		//envMap
		CubeTexture::Ptr	m_envMap{ nullptr };

		//normal
		Texture::Ptr		m_normalMap{ nullptr };

		//specularMap
		Texture::Ptr		m_specularMap{ nullptr };

	protected:
		ID					m_id{ 0 };
		std::string			m_type;
	};
}