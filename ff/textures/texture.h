/**
 * @class Texture
 * @brief Texture 表示一张纹理贴图及其相关的所有参数设置，是图形渲染中纹理资源的核心描述类。
 *
 * 该类封装了纹理的尺寸、像素格式、数据类型、采样方式、包装方式等属性，同时还持有图片源数据 `Source::Ptr`。
 * 它支持对贴图的用途（如作为采样纹理或渲染目标）进行标识，也提供了深拷贝接口 `clone` 以复制纹理配置。
 *
 * 纹理的更新状态由 `mNeedsUpdate` 标志表示，任何尺寸变更、采样参数变更或图像数据变更都应设置此标志以触发重上传。
 *
 * Example usage:
 * ```cpp
 * ```
 * @note 该类只负责描述纹理的逻辑属性，不负责纹理上传或 GPU 绑定等具体行为，实际渲染引擎中应由后端驱动管理。
 * @note `mFormat` 表示外部图片数据的格式；`mInternalFormat` 表示 GPU 内部存储格式，可设为不同以优化性能。
 * @note 所有纹理对象都有唯一 ID（`mID`）用于资源管理。
 *
 * @see ff::Source, ff::TextureWrapping, ff::TextureFilter, ff::TextureFormat, ff::TextureUsage, ff::TextureType
 * @author qiang.guo
 * @date 2025-06-08
 */


#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "source.h"

namespace ff {

	//负责给出纹理贴图描述信息
	class Texture {
	public:
		using Ptr = std::shared_ptr<Texture>;
		static Ptr create(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::LinearFilter,
			const TextureFilter& minFilter = TextureFilter::LinearFilter,
			const TextureFormat& format = TextureFormat::RGBA
		)
		{
			return std::make_shared<Texture>(
				width,
				height,
				dataType,
				wrapS,
				wrapT,
				wrapR,
				magFilter,
				minFilter,
				format
			);
		}

		Texture(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::LinearFilter,
			const TextureFilter& minFilter = TextureFilter::LinearFilter,
			const TextureFormat& format = TextureFormat::RGBA
		) noexcept;

		~Texture() noexcept;

		ID getID() const noexcept { return m_id };

		TextureUsage getUsage() const noexcept { return m_usage };

		virtual Texture::Ptr clone() noexcept;


	public:
		TextureFilter		m_minFilter{ TextureFilter::LinearFilter };
		TextureFilter		m_magFilter{ TextureFilter::LinearFilter };

		TextureWrapping		m_wrapS{ TextureWrapping::RepeatWrapping };
		TextureWrapping		m_wrapT{ TextureWrapping::RepeatWrapping };
		TextureWrapping		m_wrapR{ TextureWrapping::RepeatWrapping };

		//像素格式信息
		//Format 即输入的图片像素格式，internalFormat即在shader采样的时候，内存中存放的格式
		TextureFormat		m_format{ TextureFormat::RGBA }; 
		TextureFormat		m_internalFormat{ TextureFormat::RGBA };

		DataType			m_dataType{ DataType::UnsignedByteType }; //每个颜色通道的数值格式

		uint32_t			m_width{ 0 };
		uint32_t			m_height{ 0 };

		Source::Ptr			m_source{ nullptr };

		bool				m_needUpdate{ true }; //要么长宽变了，要么参数变了，要么数据变了

		TextureType			m_textureType{ TextureType::Texture2D }; //纹理类型，平面纹理，立方体贴图，纹理数组

		TextureUsage		m_usage{ TextureUsage::SamplerTexture }; //本纹理用于何方，贴图，画布（colorRendertarget/colorAttachment）

	protected:
		ID					m_id{ 0 };
	};
}