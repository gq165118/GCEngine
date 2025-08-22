#pragma once
#include "../../global/base.h"
#include "../../global/constant.h"
#include "driverTextures.h"
#include "../shaders/uniformsLib.h"

namespace ff {

	class DriverUniforms;

	//一切uniform类型的根类
	class UniformBase {
	public:
		using Ptr = std::shared_ptr<UniformBase>;
		UniformBase() noexcept;

		virtual ~UniformBase() noexcept;

		//uniform的名字
		std::string m_ID;

		//完成更新uniform这件事儿
		virtual void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms
		){}
	};

	//1 存储location type类型
	//2 提供setValue的接口，并且在其内部，根据不同类型的uniform调用不同的upload

	class SingleUniform : public UniformBase {
	public:
		using Ptr = std::shared_ptr<SingleUniform>;
		static Ptr create(const std::string& id, const GLint& location, const GLenum& type)
		{
			return std::make_shared<SingleUniform>(id, location, type);
		}

		SingleUniform(const std::string& id, const GLint& location, const GLenum& type);

		~SingleUniform() noexcept;

	public:
		GLint m_location{ 0 };
		GLenum m_type;

	public:
		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms
		) override;

		void uploadTexture(
			const std::shared_ptr<DriverUniforms>& driverUniforms,
			const DriverTextures::Ptr& textures,
			const std::any& value
		);

		//将T类型的变量value，传输到location所代表的uniform里面
		template<typename T>
		void upload(const T& value) {};

		//模板偏特化
		template<>
		void upload<float>(const float& value);

		template<>
		void upload<glm::vec2>(const glm::vec2& value);

		template<>
		void upload<glm::vec3>(const glm::vec3& value);

		template<>
		void upload<glm::vec4>(const glm::vec4& value);

		template<>
		void upload<int>(const int& value);

		template<>
		void upload<glm::ivec2>(const glm::ivec2& value);

		template<>
		void upload<glm::ivec3>(const glm::ivec3& value);

		template<>
		void upload<glm::ivec4>(const glm::ivec4& value);

		template<>
		void upload<bool>(const bool& value);

		template<>
		void upload<glm::bvec2>(const glm::bvec2& value);

		template<>
		void upload<glm::bvec3>(const glm::bvec3& value);

		template<>
		void upload<glm::bvec4>(const glm::bvec4& value);

		template<>
		void upload<glm::mat2>(const glm::mat2& value);

		template<>
		void upload<glm::mat3>(const glm::mat3& value);

		template<>
		void upload<glm::mat4>(const glm::mat4& value);
		
	};

	//纯粹的数组类型的Uniform
	class PureArrayUniform : public UniformBase 
	{
	public:
		using Ptr = std::shared_ptr<PureArrayUniform>;
		static Ptr create(const std::string& id, const GLint& location, const GLenum& type)
		{
			return std::make_shared<PureArrayUniform>(id, location, type);
		}

		PureArrayUniform(const std::string& iid, const GLint& location, const GLenum& type);

		~PureArrayUniform();

		GLint	m_location{ 0 };
		GLenum	m_type;
		GLint	m_size{ 0 };

	public:
		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms
		) override;

		void uploadTexture2DArray(
			const std::shared_ptr<DriverUniforms>& driverUniforms,
			const DriverTextures::Ptr& textures,
			const std::any& value
		);

		template<typename T>
		void upload(const T* value) {}

		template<>
		void upload<float>(const float* value);

		template<>
		void upload<glm::vec2>(const glm::vec2* value);

		template<>
		void upload<glm::vec3>(const glm::vec3* value);

		template<>
		void upload<glm::vec4>(const glm::vec4* value);

		template<>
		void upload<int>(const int* value);

		template<>
		void upload<glm::ivec2>(const glm::ivec2* value);

		template<>
		void upload<glm::ivec3>(const glm::ivec3* value);

		template<>
		void upload<glm::ivec4>(const glm::ivec4* value);

		//no bool bvec2 bvec3 bvec4 we use int instead

		template<>
		void upload<glm::mat2>(const glm::mat2* value);

		template<>
		void upload<glm::mat3>(const glm::mat3* value);

		template<>
		void upload<glm::mat4>(const glm::mat4* value);

	};

	class UniformContainer 
	{
	public:
		using Ptr = std::shared_ptr<UniformContainer>;
		std::unordered_map<std::string, UniformBase::Ptr> m_uniformMap;

		virtual void fake() {}
	};

	class StructuredUniform :public UniformBase, public UniformContainer 
	{
		using Ptr = std::shared_ptr<StructuredUniform>;
		

		StructuredUniform(const std::string& id) noexcept;

		~StructuredUniform();

		void setValue(
			const std::any& value,
			const DriverTextures::Ptr& textures,
			const std::shared_ptr<DriverUniforms>& driverUniforms) override;
	};

	class DriverUniforms :public UniformContainer, public std::enable_shared_from_this<DriverUniforms> {
	public:
		

		//texture slots
	




		//跟Texture Array相关 uniform sampler2D texs[10];


		//返回n个可以使用的textureUnits
		

	private:
		//key:某一个uniform sampler2D tex;变量的location
		//value：GL_TEXTUREXXXX
		

		//uniform sampler2D texs[10];
		//name: texs[0] 
		//size:10
		//需要给texs分配十个slot（units）
		//每个purearrayuniform只有一个location
		//key：PureArrayUniform这种类型的texture数组的location 
		//value：数组，为这个texture数组当中的所有textures按序分配的textureUnits
		

		//用于记录当前已经分配到了哪一个TextureUnit
		
	};


	template<>
	void SingleUniform::upload<float>(const float& value)
	{
		glUniform1f(m_location, value);
	}

	template<>
	void SingleUniform::upload<glm::vec2>(const glm::vec2& value)
	{
		glUniform2fv(m_location, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::vec3>(const glm::vec3& value)
	{
		glUniform3fv(m_location, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::vec4>(const glm::vec4& value)
	{
		glUniform4fv(m_location, 1, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<int>(const int& value)
	{
		glUniform1i(m_location, value);
	}

	template<>
	void SingleUniform::upload<glm::ivec2>(const glm::ivec2& value)
	{
		glUniform2i(m_location, value.x, value.y);
	}

	template<>
	void SingleUniform::upload<glm::ivec3>(const glm::ivec3& value)
	{
		glUniform3i(m_location, value.x, value.y, value.z);
	}

	template<>
	void SingleUniform::upload<glm::ivec4>(const glm::ivec4& value)
	{
		glUniform4i(m_location, value.x, value.y, value.z, value.w);
	}

	template<>
	void SingleUniform::upload<bool>(const bool& value)
	{
		int v = value;
		glUniform1i(m_location, v);
	}

	template<>
	void SingleUniform::upload<glm::bvec2>(const glm::bvec2& value)
	{
		glm::ivec2 v = value;
		glUniform2i(m_location, v.x, v.y);
	}

	template<>
	void SingleUniform::upload<glm::bvec3>(const glm::bvec3& value)
	{
		glm::ivec3 v = value;
		glUniform3i(m_location, v.x, v.y, v.z);
	}

	template<>
	void SingleUniform::upload<glm::bvec4>(const glm::bvec4& value)
	{
		glm::ivec4 v = value;
		glUniform4i(m_location, v.x, v.y, v.z, v.w);
	}

	template<>
	void SingleUniform::upload<glm::mat2>(const glm::mat2& value)
	{
		glUniformMatrix2fv(m_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::mat3>(const glm::mat3& value)
	{
		glUniformMatrix3fv(m_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	void SingleUniform::upload<glm::mat4>(const glm::mat4& value)
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, glm::value_ptr(value));
	}


	template<>
	void PureArrayUniform::upload<float>(const float* value)
	{
		glUniform1fv(m_location, m_size, value);
	}

	template<>
	void PureArrayUniform::upload<glm::vec2>(const glm::vec2* value)
	{
		glUniform2fv(m_location, m_size, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::vec3>(const glm::vec3* value)
	{
		glUniform3fv(m_location, m_size, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::vec4>(const glm::vec4* value)
	{
		glUniform4fv(m_location, m_size, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<int>(const int* value)
	{
		glUniform1iv(m_location, m_size, value);
	}

	template<>
	void PureArrayUniform::upload<glm::ivec2>(const glm::ivec2* value)
	{
		glUniform2iv(m_location, m_size, glm::value_ptr(value[0]));
	}

	template<>
	void PureArrayUniform::upload<glm::ivec3>(const glm::ivec3* value);

	template<>
	void PureArrayUniform::upload<glm::ivec4>(const glm::ivec4* value);

	//no bool bvec2 bvec3 bvec4 we use int instead

	template<>
	void PureArrayUniform::upload<glm::mat2>(const glm::mat2* value);

	template<>
	void PureArrayUniform::upload<glm::mat3>(const glm::mat3* value);

	template<>
	void PureArrayUniform::upload<glm::mat4>(const glm::mat4* value);


}