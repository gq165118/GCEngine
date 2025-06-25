/**
 * @class DriverAttribute
 * @brief 表示 GPU 中的顶点属性缓冲（VBO）句柄，用于管理单个 Attribute 的 OpenGL 缓冲。
 *
 * 本类用于抽象 GPU 端对一个顶点属性数据（如 Position、Normal 等）的存储管理，
 * 通过 `m_handle` 成员记录 OpenGL 分配的 VBO ID，供渲染阶段绑定和更新。
 *
 * @note 通常不单独使用，应通过 DriverAttributes 进行统一管理。
 *
 * @author qiang.guo
 * @date 2025-06-17
 */


 /**
  * @class DriverAttributes
  * @brief 管理所有 Attribute 对应的 GPU 缓冲区（VBO）对象，负责上传、更新和释放资源。
  *
  * `DriverAttributes` 是渲染驱动层与资源层之间的桥梁，用于将 CPU 上的 Attribute<T>
  * 映射成 GPU 端的 VBO，通过 OpenGL 接口进行缓存创建和数据传输。
  *
  * 主要职责：
  * - 为 Attribute 创建 GPU 缓冲（VBO）并存储为 DriverAttribute
  * - 检测 Attribute 是否需要更新，并进行 BufferSubData 或 BufferData 更新
  * - 响应 Attribute 析构事件（attributeDispose）并移除对应的缓冲资源
  *
  * Example usage:
  * @code
  * auto driverAttr = driverAttributes->update(attrPosition, BufferType::ArrayBuffer);
  * glBindBuffer(GL_ARRAY_BUFFER, driverAttr->m_handle);
  * @endcode
  *
  * @note 模板接口 `update<T>()` 和 `get<T>()` 适用于任意类型的 Attribute。
  *
  * @see Attribute, DriverAttribute, EventDispatcher
  * @author qiang.guo
  * @date 2025-06-17
  */


#pragma once
#include "../../global/base.h"
#include "../../core/attribute.h"
#include "../../global/eventDispatcher.h"

namespace ff
{
	class DriverAttribute
	{
	public:
		using Ptr = std::shared_ptr<DriverAttribute>;
		static Ptr create()
		{
			return std::make_shared<DriverAttribute>();
		}

		DriverAttribute() noexcept;

		~DriverAttribute() noexcept;


		GLuint	m_handle{ 0 };  //vbo ebo
	};

	class DriverAttributes
	{
	public:
		using Ptr = std::shared_ptr<DriverAttributes>;
		static Ptr create()
		{
			return std::make_shared<DriverAttributes>();
		}

		using DriverAttributesMap = std::unordered_map<ID, DriverAttribute::Ptr>;

		DriverAttributes() noexcept;
		~DriverAttributes() noexcept;
		
		template<typename T>
		DriverAttribute::Ptr update(
			const std::shared_ptr<Attribute<T>>& attribute,
			const BufferType& bufferType
		) noexcept;
		
		template<typename T>
		DriverAttribute::Ptr get(const std::shared_ptr<Attribute<T>>& attribute) noexcept;

		void remove(ID attributeID) noexcept;

		void onAttributeDispose(const EventBase::Ptr& e);

	private:
		DriverAttributesMap m_attributes{};
	};

	template<typename T>
	DriverAttribute::Ptr DriverAttributes::update(
		const std::shared_ptr<Attribute<T>>& attribute,
		const BufferType& bufferType
	) noexcept
	{
		DriverAttribute::Ptr dattribute = nullptr;

		// 1 如果本Attribute没有对应的DriverAttribute，就为其生成，且更新数据
		auto iter = m_attributes.find(attribute->getID());
		if (iter != m_attributes.end())
		{
			dattribute = iter->second;
		}
		else  //如果没有找到，则创建一个新的DriverAttribute
		{
			dattribute = DriverAttribute::create();

			auto date = attribute->getData();

			glGenBuffers(1, &dattribute->m_handle);

			glBindBuffer(toGL(bufferType), dattribute->m_handle);

			//vbo内存开辟以及数据灌入
			glBufferData(toGL(bufferType), date.size() * sizeof(T), date.data(), toGL(attribute->getBufferAllocType()));
			glBindBuffer(toGL(bufferType), 0);
			
			m_attributes.insert(std::make_pair(attribute->getID(), dattribute));

			attribute->clearUpdateRange();
			attribute->clearNeedsUpdate();
		}

		//如果原来就存在DriverAttribute,那就检查是否需要更新
		if (attribute->getNeedUpdate())
		{
			attribute->clearNeedsUpdate();

			auto updateRange = attribute->getUpdateRange();
			auto data = attribute->getData();

			//如果用户确实指定的更新的range
			if (updateRange.m_count > 0)
			{
				glBufferSubData(
					toGL(bufferType),
					updateRange.m_offset * sizeof(T),
					updateRange.m_count * sizeof(T),
					data.data() + updateRange.m_offset * sizeof(T));
			}
			else
			{
				glBufferData(toGL(bufferType), data.size() * sizeof(T), data.data(), toGL(attribute->getBufferAllocType()));

			}

			glBindBuffer(toGL(bufferType), 0);

			attribute->clearUpdateRange();

		}

		return dattribute;

	}

	template<typename T>
	DriverAttribute::Ptr DriverAttributes::get(const std::shared_ptr<Attribute<T>>& attribute) noexcept
	{
		auto iter = m_attributes.find(attribute->getID());
		if (iter != m_attributes.end())
		{
			return iter->second;
		}

		return nullptr;
	}

}