/**
 * @class DepthTexture
 * @brief DepthTexture 表示一张专用于深度存储的纹理贴图，用于渲染管线中的深度测试、阴影映射等场景。
 *
 * 该类继承自 `Texture`，专门用于描述深度纹理资源的逻辑属性，包括尺寸、数据类型、
 * 包装方式、采样方式及 GPU 内部格式等配置。默认使用 `TextureFormat::DepthFormat`，
 * 适用于常见的 2D 深度纹理创建需求。
 *
 * DepthTexture 提供了静态工厂方法 `create` 以便简化对象构造，同时保持与引擎资源管理体系一致的
 * 引用计数（`std::shared_ptr`）使用方式。构造函数允许灵活指定内部格式、数据类型以及纹理包装/过滤方式。
 *
 * 深度纹理通常在以下场景中使用：
 * - 深度测试（Depth Test）
 * - 阴影贴图（Shadow Mapping）
 * - 延迟渲染（Deferred Rendering）中的深度 G-Buffer
 * - 屏幕空间算法（例如 SSAO）
 *
 * @note DepthTexture 仅负责描述深度纹理的逻辑配置，不负责 GPU 上传、绑定、FBO attach 等渲染后端操作。
 *       实际上传应由后端驱动层统一处理。
 * @note 与 `Texture` 基类一样，纹理的尺寸或参数变更应设置 `mNeedsUpdate` 标志以触发重上传。
 * @note 默认数据类型为 `FloatType`，适用于大多数深度格式；包装方式及过滤方式默认为 Nearest，
 *       以确保深度采样的准确性。
 *
 * Example usage:
 * ```cpp
 * // 创建一张 1024 × 1024 的深度纹理，用于 Shadow Map
 * ff::DepthTexture::Ptr depthTex = ff::DepthTexture::create(
 *     1024, 1024,
 *     ff::DataType::FloatType,
 *     ff::TextureWrapping::ClampToEdgeWrapping,
 *     ff::TextureWrapping::ClampToEdgeWrapping
 * );
 * ```
 *
 * @see ff::Texture, ff::TextureFormat, ff::TextureFilter, ff::TextureWrapping, ff::TextureType
 * @author qiang.guo
 * @date 2025-12-09
 */


#pragma once
#include "texture.h"

namespace ff {

	class DepthTexture :public Texture {
	public:
		using Ptr = std::shared_ptr<DepthTexture>;
		static Ptr create(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::FloatType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::DepthFormat,
			const TextureType& textureType = TextureType::Texture2D)
		{
			return std::make_shared <DepthTexture>(
				width, 
				height,
				dataType,
				wrapS,
				wrapT,
				wrapR,
				magFilter,
				minFilter,
				format,
				textureType
				);
		}

		DepthTexture(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::FloatType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::DepthFormat,
			const TextureType& textureType = TextureType::Texture2D) noexcept;

		~DepthTexture() noexcept;

	};
}