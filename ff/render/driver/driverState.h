/**
 * @class DriverState
 * @brief 统一管理渲染管线中的可配置状态（视口、程序、FBO、深度、颜色、混合、光栅化）。
 *
 * 本类封装 OpenGL 常用渲染状态的设置与缓存，提供面向材质与渲染目标的便捷接口，
 * 以减少无效状态切换并提升渲染阶段的可读性与可维护性。
 *
 * 功能概览：
 * - 视口设置：viewport()
 * - 着色器程序切换与缓存：useProgram()
 * - 材质绑定与状态同步：setMaterial()
 * - 帧缓冲绑定：bindFrameBuffer()
 * - 清屏颜色设置与查询：setClearColor() / getClearColor()
 * - 颜色混合配置：setBlending()
 * - 深度测试与写入配置：setDepth()
 *
 * 状态分组说明：
 * - RasterState：面剔除与正面朝向配置（mSide、mFrontFace）
 * - BlendingState：混合类型、透明标志、RGB/Alpha 源因子、目标因子与方程
 * - DepthState：深度测试开关、写入掩码、比较函数与深度清空值
 * - ColorState：清屏颜色（mClearColor）
 *
 * 设计要点：
 * - 内部缓存当前状态（程序、视口、FBO 与各类管线状态），避免重复调用底层 GL 接口。
 * - 通过枚举（BlendingType、BlendingFactor、BlendingEquation、CompareFunction 等）
 *   屏蔽底层常量，提升可移植性与可读性。
 *
 * 使用建议：
 * 1) 在渲染批次开始时先绑定目标 FBO 并设置 viewport，再配置清屏颜色与深度状态。
 * 2) 切换着色器后调用 setMaterial() 同步与材质相关的混合、深度等状态。
 * 3) 针对透明对象启用合适的混合模式与绘制顺序；不透明对象建议关闭透明混合并开启深度写入。
 *
 * 与其他组件的关系：
 * - 与 Material 协同：setMaterial() 会将材质层的状态意图下发至 DriverState。
 * - 与 DriverRenderTargets 协同：bindFrameBuffer() 负责渲染目标切换与状态缓存。
 * 
 * 默认值说明：
 * - 清屏颜色默认 vec4(0,0,0,1)。
 * - 深度清空值默认 -1.0 表示未配置或按外部策略处理。
 *
 * @note 本类不直接发起绘制调用，主要负责状态配置与缓存。实际绘制应由更高层渲染流程驱动。
 *
 * @author qiang.guo
 * @date 2025-08-29
 */

#pragma once
#include "../../global/base.h"
#include "../../material/material.h"
#include "../renderTarget.h"
#include "driverRenderTargets.h"

namespace ff {

	//配置渲染管线
	//1 深度检测配置
	//2 颜色混合配置
	//3 光栅化配置
	//4 其他相关配置
	class DriverState {
	public:
		struct RasterState {
			Side		mSide{ Side::None };
			FrontFace	mFrontFace{ FrontFace::None };
		};

		struct ColorState {
			glm::vec4 mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		};

		struct BlendingState {
			BlendingType		mBlendingType{ BlendingType::None };
			bool				mTransparent{ false };
			BlendingFactor		mBlendSrc{ BlendingFactor::None };
			BlendingFactor		mBlendDst{ BlendingFactor::None };
			BlendingEquation	mBlendEquation{ BlendingEquation::None };
			BlendingFactor		mBlendSrcAlpha{ BlendingFactor::None };
			BlendingFactor		mBlendDstAlpha{ BlendingFactor::None };
			BlendingEquation	mBlendEquationAlpha{ BlendingEquation::None };
		};

		struct DepthState {
			bool			mDepthTest{ false };
			bool			mDepthWrite{ false };
			CompareFunction mDepthFunction{ CompareFunction::None };
			double			mDepthClearColor{ -1.0f };
		};

		using Ptr = std::shared_ptr<DriverState>;
		static Ptr create() { return std::make_shared<DriverState>(); }

		DriverState() noexcept;

		~DriverState() noexcept;

		void viewport(const glm::vec4& viewport) noexcept;

		bool useProgram(GLuint program) noexcept;

		void setMaterial(const Material::Ptr& material) noexcept;

		void bindFrameBuffer(const GLuint& frameBuffer) noexcept;

		void setClearColor(float r, float g, float b, float a) noexcept;

		void setBlending(
			BlendingType		blendingType,
			bool				transparent = false,
			BlendingFactor		blendSrc = BlendingFactor::SrcAlpha,
			BlendingFactor		blendDst = BlendingFactor::OneMinusSrcAlpha,
			BlendingEquation	blendEquation = BlendingEquation::AddEquation,
			BlendingFactor		blendSrcAlpha = BlendingFactor::SrcAlpha,
			BlendingFactor		blendDstAlpha = BlendingFactor::OneMinusSrcAlpha,
			BlendingEquation	blendEquationAlpha = BlendingEquation::AddEquation
		) noexcept;
		
		void setDepth(
			bool			depthTest,
			bool			depthWrite,
			CompareFunction depthFunction,
			double			depthClearColor
		) noexcept;

		glm::vec4 getClearColor() const noexcept;

	private:
		void setBlendingInternal(
			bool				transparent,
			BlendingFactor		blendSrc,
			BlendingFactor		blendDst,
			BlendingEquation	blendEquation,
			BlendingFactor		blendSrcAlpha,
			BlendingFactor		blendDstAlpha,
			BlendingEquation	blendEquationAlpha
		) noexcept;

	private:
		RasterState		mCurrentRaster;
		BlendingState	mCurrentBlending;
		DepthState		mCurrentDepth;
		ColorState		mCurrentColor;

		GLuint	mCurrentProgram{ 0 };
		glm::vec4 mCurrentViewport{};
		GLuint	mCurrentFrameBuffer{ 0 };
	};
}