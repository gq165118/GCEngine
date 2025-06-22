/**
 * @class Attribute
 * @brief Attribute 是对 Mesh 顶点属性的一种通用抽象模板，用于存储并管理如 Position、Index 等类型的数据。
 *
 * 它支持对任意类型的属性值进行统一管理，例如 float 类型的顶点位置、uint32_t 类型的索引等，并支持按维度（X、Y、Z）进行访问和修改。
 * 每个 Attribute 绑定唯一 ID，并自动管理数据更新标记和局部更新范围。
 *
 * Example usage:
 * ```cpp
 * using Attributef = ff::Attribute<float>;
 * std::vector<float> positions = {0.f, 0.f, 0.f, 1.f, 1.f, 1.f}; // 两个点
 * auto attr = Attributef::carete(positions, 3);
 * attr->setX(0, 0.5f);
 * float x = attr->getX(0);
 * ```
 *
 * @note 本类通过事件系统在析构时广播 "attributeDispose" 消息，配合事件监听机制实现资源生命周期通知。
 * @note 非线程安全。外部需自行保证并发安全。
 *
 * @tparam T 属性中存储的数据类型（如 float、uint32_t）。
 * @author qiang.guo
 * @date 2025-06-07
 */


#pragma once
#include "../global/base.h"
#include "../global/constant.h"
#include "../tools/identity.h"
#include "../global/eventDispatcher.h"

namespace ff
{
	//对于每个Mesh，我们将其所有顶点的某个Attribute共同存储成一个数组，比如Position就是一个float类型的数组
	//每个Attribute有可能数字类型不同，比如Position需要float，index需要uint32_t
	template<typename T>
	class Attribute : public std::enable_shared_from_this<Attribute<T>> 
	{
	public:
		using Ptr = std::shared_ptr<Attribute<T>>;
		static Ptr carete(const std::vector<T>& data, uint32_t iiemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer)
		{
			return std::make_shared<Attribute<T>>(data, itemSize, bufferAllocType);
		}

		Attribute(const std::vector<T>& date, uint32_t itemSize, BufferAllocType bufferAllocType = BufferAllocType::StaticDrawBuffer) noexcept;

		~Attribute() noexcept;

		void setX(const uint32_t& index, T value) noexcept;

		void setY(const uint32_t& index, T value) noexcept;

		void setZ(const uint32_t& index, T value) noexcept;

		//得到第index个顶点的本attribute的x值
		T getX(const uint32_t& index) noexcept;
		
		T getY(const uint32_t& index) noexcept;
		
		T getZ(const uint32_t& index) noexcept;

		auto getID() const noexcept { return m_id; }

		auto getData() const noexcept { return m_data; }

		auto getCount() const noexcept { return m_count; }

		auto getItemSize() const noexcept { return m_itemSize; }

		auto getNeedUpdate() const noexcept { return m_needUpdate; }

		void clearNeedsUpdate() noexcept { m_needUpdate = false; }

		auto getBufferAllocType() const noexcept { return m_bufferAllocType; }

		Range getUpdateRange() const noexcept { return m_updateRange; }

		void clearUpdateRange() noexcept { m_updateRange.m_offset = 0; m_updateRange.m_count = -1; }

		auto getDataType() const noexcept { return m_dataType; }

	private:
		ID				m_id{ 0 };
		std::vector<T>	m_data{};	//数据数组
		uint32_t		m_itemSize{ 0 }; //多少个数据为一个顶点的Attribute
		uint32_t		m_count{ 0 };  //本attribute的数据，包含了多少个顶点的数据

		BufferAllocType m_bufferAllocType{ BufferAllocType::StaticDrawBuffer };

		DataType		m_dataType{ DataType::FloatType }; //记录本attribute的数据类型

		bool			m_needUpdate{ true };
		Range			m_updateRange{}; //假设数组长度为300个float类型的数组，本次更新，可以只更新55-100个float数据

	};

	using Attributef = Attribute<float>;
	using Attributei = Attribute<uint32_t>;

	template<typename T>
	Attribute<T>::Attribute(const std::vector<T>& data, uint32_t itemSize, BufferAllocType bufferAllocType) noexcept
	{
		m_id = Identity::generateID();

		m_data = data;
		m_itemSize = itemSize;

		m_count = static_cast<uint32_t>(m_data.size() / itemSize);

		m_bufferAllocType = bufferAllocType;

		m_dataType = toDataType<T>();
	}

	template<typename T>
	Attribute<T>::~Attribute() noexcept
	{
		//发送消息给到各类监听的函数，通知他们哪个attribute已经消亡
		EventBase::Ptr e = EventBase::create("attributeDispose");
		e->m_target = this;
		e->m_pUserData = &m_id;

		EventDispatcher::getInstance()->dispatchEvent(e);
	}

	template<typename T>
	void Attribute<T>::setX(const uint32_t& index, T value) noexcept
	{
		assert(index < m_count);

		//float vector: a b c value e f g h i j
		//假设index = 1 itemsize=3
		m_data[index * m_itemSize] = value;
		m_needUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setY(const uint32_t& index, T value) noexcept
	{
		assert(index < m_count);

		mData[index * mItemSize + 1] = x;
		mNeedsUpdate = true;
	}

	template<typename T>
	void Attribute<T>::setZ(const uint32_t& index, T value) noexcept
	{
		assert(index < m_count);

		//float vector: a b c d e value g h i j
		//假设index = 1 itemsize=3
		m_data[index * m_itemSize + 2] = x;
		mNeedsUpdate = true;
	}

	template<typename T>
	T Attribute<T>::getX(const uint32_t& index) noexcept 
	{
		assert(index < m_count);
		return m_data[index * m_itemSize];
	}

	template<typename T>
	T Attribute<T>::getY(const uint32_t& index) noexcept 
	{
		assert(index < m_count);
		return m_data[index * m_itemSize + 1];
	}

	template<typename T>
	T Attribute<T>::getZ(const uint32_t& index) noexcept 
	{
		assert(index < m_count);
		return m_data[index * m_itemSize + 2];
	}

}