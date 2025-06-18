/**
 * @class CubeTexture
 * @brief CubeTexture 表示立方体贴图纹理，是对六面纹理图像的统一封装。
 *
 * CubeTexture 继承自 Texture，用于实现环境映射（如天空盒、反射、折射等）。
 * 其内部维护六个 `Source::Ptr` 图像资源，分别代表立方体贴图的六个面：
 * +X, -X, +Y, -Y, +Z, -Z，对应索引 0-5。
 *
 * 与普通 2D 纹理相比，它的纹理类型（`mTextureType`）默认被设置为 `TextureType::TextureCube`，
 * 并具备与之匹配的宽高、过滤方式、格式等属性。
 *
 * 使用 `create()` 静态工厂函数可以快速创建具有默认参数的立方体纹理实例。
 *
 * Example usage:
 * ```cpp
 * //
 * ```
 * @note 使用时需确保 `mSources` 六个面均已正确赋值，否则在上传至 GPU 时可能出现错误。
 * @note 并非所有渲染后端都支持非正方形立方体贴图，推荐 width == height。
 * @note 默认使用 NearestFilter 做采样过滤，适合低开销场景，必要时应设为 LinearFilter。
 *
 * @see ff::Texture, ff::Source, ff::TextureType::TextureCube, CubeTexture::CUBE_TEXTURE_COUNT
 * @author qiang.guo
 * @date 2025-06-08
 */


#pragma once 
#include "../global/base.h"
#include "../global/constant.h"
#include "texture.h"

namespace ff 
{

	class CubeTexture :public Texture 
	{
	public:
		static const uint32_t CUBE_TEXTURE_COUNT = 6;
		using Ptr = std::shared_ptr<CubeTexture>;
		static Ptr create(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::RGBA
		)
		{
			return std::make_shared<CubeTexture>(
				width,
				height,
				dataType,
				wrapS,
				wrapT,
				wrapR,
				magFilter,
				minFilter,
				format);
		}

		CubeTexture(
			const uint32_t& width,
			const uint32_t& height,
			const DataType& dataType = DataType::UnsignedByteType,
			const TextureWrapping& wrapS = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapT = TextureWrapping::RepeatWrapping,
			const TextureWrapping& wrapR = TextureWrapping::RepeatWrapping,
			const TextureFilter& magFilter = TextureFilter::NearestFilter,
			const TextureFilter& minFilter = TextureFilter::NearestFilter,
			const TextureFormat& format = TextureFormat::RGBA
		) noexcept;

		~CubeTexture() noexcept;

		Source::Ptr m_sources[CUBE_TEXTURE_COUNT] = { nullptr };
	};
}