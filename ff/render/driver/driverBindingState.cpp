#include "driverBindingState.h"
#include "../../core/geometry.h"

namespace ff {

	DriverBindingState::DriverBindingState() noexcept {}

	DriverBindingState::~DriverBindingState() noexcept 
	{
		if (m_vao) 
		{
			glDeleteVertexArrays(1, &m_vao);
		}
	}

	DriverBindingStates::DriverBindingStates(const DriverAttributes::Ptr& attributes) 
	{
		m_attributes = attributes;
	}

	DriverBindingStates::~DriverBindingStates() {}

	//寻找当前geometry是否曾经生成过一个DriverBindingState，如果没有，则新生成一个，否则把以往的交回去
	DriverBindingState::Ptr DriverBindingStates::getBindingState(const Geometry::Ptr& geometry) noexcept 
	{
		DriverBindingState::Ptr state = nullptr;
		auto iter = m_bindingStates.find(geometry->getID());
		if (iter == m_bindingStates.end())
		{
			//如果没有，则新生成一个
			iter = m_bindingStates.insert(std::make_pair(geometry->getID(), createBindingState(createVAO()))).first;
		}

		state = iter->second;

		return state;
	}

	// 生成并管理vbo、设置绑定状态、负责vao绑定状态的缓存
	void DriverBindingStates::setup(
		const Geometry::Ptr& geometry,
		const Attributei::Ptr& index
	) 
	{
		bool updateBufferLayout = false;

		auto state = getBindingState(geometry);
		
		if (m_currentBindingState != state)
		{
			m_currentBindingState = state;
			bindVAO(state->m_vao);
		}

		updateBufferLayout = needsUpdate(geometry, index);
		if (updateBufferLayout)
		{
			saveCache(geometry, index);  //保存到当前vao
		}
		
		//这里创建ebo，index顶点缩影数据的处理与vao平级
		if (index != nullptr)
		{
			m_attributes->update(index, BufferType::IndexBuffer);
		}

		//更新挂钩关系
		if (updateBufferLayout)
		{
			setupVertexAttributes(geometry);

			if (index != nullptr)
			{
				//从DriverAttributes里面拿出来indexAttribute对应的DriverAttribute
				auto bkInex = m_attributes->get(index);
				if (bkInex != nullptr)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bkInex->m_handle);
				}
			}
		}

	}

	DriverBindingState::Ptr DriverBindingStates::createBindingState(GLuint vao) noexcept 
	{
		auto bindingState = DriverBindingState::create();
		bindingState->m_vao = vao;

		return bindingState;
	}

	//绑定VAO之后，需要使用glVertexAttribPointer等函数，对vao与各个vbo之间的挂钩关系做设置
	//如果第一帧绘制完毕之后，第二帧继续绘制同样的VAO，那么这个挂钩关系的设置，就不需要做第二次
	bool DriverBindingStates::needsUpdate(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept {
		//id->名字，value->attribute id
		auto cachedAttributes = m_currentBindingState->m_attributes;

		//id->名字，value->attribute对象
		auto  geometryAttributes = geometry->getAttributes();

		uint32_t attributeNum = 0;
		for (const auto& iter : geometryAttributes)
		{
			auto key = iter.first;
			auto geometryAttribute = iter.second;

			//1 从缓存里面寻找，但凡有一个attribute没找到，说明就不一样了
			auto cachedIter = cachedAttributes.find(key);
			if (cachedIter == cachedAttributes.end())
			{
				return true;
			}

			//2 从缓存中确实找到了attribute，那么就要对比id是否一致，确保同名的attribute是否是同一个
			auto cachedAttribute = cachedIter->second;
			if (cachedAttribute != geometryAttribute->getID())
			{
				return true;
			}

			attributeNum++;
		}

		//3 如果旧的geometry有3个属性，心得geometry去掉了一个也需要更新
		if (m_currentBindingState->m_attributeNum != attributeNum)
		{
			return true;
		}

		//indexAttribute 如果不同，仍然需要重新挂钩
		if (index != nullptr && m_currentBindingState->m_indexID != index->getID())
		{
			return true;
		}
	
		return false;
	}

	void DriverBindingStates::saveCache(const Geometry::Ptr& geometry, const Attributei::Ptr& index) noexcept 
	{
		//首先清空掉bindingState里面的attributes （Map）
		auto& cachedAttributes = m_currentBindingState->m_attributes;
		cachedAttributes.clear();

		auto attributes = geometry->getAttributes();
		uint32_t attributeNum = 0;

		//将geometry中的每一个attribute
		for (const auto& iter : attributes)
		{
			auto attribute = iter.second;
			cachedAttributes.insert(std::make_pair(iter.first, attribute->getID()));
			attributeNum++;
		}

		m_currentBindingState->m_attributeNum = attributeNum;
		
		if (index != nullptr)
		{
			m_currentBindingState->m_indexID = index->getID();
		}
		
	}

	//提前设计好的占坑方案 positionAttribute永远location = 0, ...
	void DriverBindingStates::setupVertexAttributes(const Geometry::Ptr& geometry) noexcept 
	{
		const auto geometryAttributes = geometry->getAttributes();

		for (const auto& iter : geometryAttributes)
		{
			auto name = iter.first;
			auto attribute = iter.second;

			//本attribute有多少个数字，比如position是3个数字
			auto itemSize = attribute->getItemSize();

			auto dataType = attribute->getDataType();

			//将attribute对应的DriverAttribute取出来
			auto bkAttribute = m_attributes->get(attribute);

			//将本attribute的location通过attribute的name取出来
			auto binddingIter = LOCATION_MAP.find(name);
			if (binddingIter == LOCATION_MAP.end())
			{
				continue;
			}

			auto binding = binddingIter->second;

			//开始向vao里面做挂钩关系
			glBindBuffer(GL_ARRAY_BUFFER, bkAttribute->m_handle);
			glEnableVertexAttribArray(binding);
			glVertexAttribPointer(binding, itemSize, toGL(dataType), false, itemSize * toSize(dataType), (void*)0);

		}
	}

	//真正的生成了一个VAO
	GLuint DriverBindingStates::createVAO() noexcept 
	{
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		return vao;
	}

	// 真正改变了OpenGL状态机，使之绑定当前的vao
	void DriverBindingStates::bindVAO(GLuint vao) noexcept 
	{
		glBindVertexArray(vao);
	}

	void DriverBindingStates::releaseStatesOfGeometry(ID geometryID) noexcept 
	{
		auto iter = m_bindingStates.find(geometryID);
		if (iter != m_bindingStates.end())
		{
			m_bindingStates.erase(iter);
		}
	}

}