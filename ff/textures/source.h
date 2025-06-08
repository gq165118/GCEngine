/**
 * @class Source
 * @brief Source 是图像资源的基础容器类，用于封装一张图片加载后的原始数据及其元信息。
 *
 * 该类主要用于表示一张已经加载到内存中的图片，包括其宽高、像素数据（以 byte 向量存储）、是否需要更新的标志等信息。
 *
 * 由于可能有多个纹理（Texture）引用同一个图片数据，`Source` 内部维护了一个哈希值 `mHashCode` 作为缓存键，
 * 并通过引用计数 `mRefCount` 跟踪当前有多少个对象持有它，从而实现资源共享与延迟释放。
 *
 * Example usage:
 * ```cpp

 * ```
 * @note `mNeedsUpdate` 标志指示该图片数据是否需要重新上传到 GPU 纹理。
 * @note 本类不涉及图片解析，只表示数据持有者，通常作为纹理资源的来源。
 *
 * @see ff::Texture, ff::HashType
 *
 * @author qiang.guo
 * @date 2025-06-08
 */


#pragma once
#include "../global/base.h"
#include "../global/constant.h"

namespace ff {

	class Source {
	public:
		using Ptr = std::shared_ptr<Source>;
		static Ptr create()
		{
			return std::make_shared<Source>();
		}

		Source() noexcept;
		~Source() noexcept;



	public:
		uint32_t			m_width{ 0 };
		uint32_t			m_height{ 0 };

		std::vector<byte>	m_data{};	//读入的图片数据
		bool				m_needUpdate{ true };

		HashType			m_hashCode{ 0 }; //存在多个texture引用了同一个source,增加缓存，每一个source都有自己的hashcode

		uint32_t			m_refCount{ 0 }; //在缓存情况下，refCount记录了本source当前被多少个对象引用，引用为0，卸载析构

	};
}