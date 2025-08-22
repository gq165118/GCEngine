#include "driverUniforms.h"

namespace ff
{
	UniformBase::UniformBase() noexcept {}

	UniformBase::~UniformBase() noexcept {}


	SingleUniform::SingleUniform(const std::string& id, const GLint& location, const GLenum& type)
	{
		m_location = location;
		m_ID = id;
		m_type = type;
	}

	SingleUniform::~SingleUniform() noexcept
	{
	}


#define UPLOAD(TYPE, VALUE)   \
	{\
		TYPE v = std::any_cast<TYPE> (VALUE); \
		upload(v);\
	}


	void SingleUniform::setValue(
		const std::any& value,
		const DriverTextures::Ptr& textures,
		const std::shared_ptr<DriverUniforms>& driverUniforms
	)
	{
		switch (m_type)
		{
		case GL_FLOAT:
			UPLOAD(float,value)
			break;
		case GL_FLOAT_VEC2:
			UPLOAD(glm::vec2, value)
				break;
		case GL_FLOAT_VEC3:
			UPLOAD(glm::vec3, value)
				break;
		case GL_FLOAT_VEC4:
			UPLOAD(glm::vec4, value)
				break;
		case GL_INT:
			UPLOAD(int, value)
				break;
		case GL_INT_VEC2:
			UPLOAD(glm::ivec2, value)
				break;
		case GL_INT_VEC3:
			UPLOAD(glm::ivec3, value)
				break;
		case GL_INT_VEC4:
			UPLOAD(glm::ivec4, value)
				break;
		case GL_BOOL:
			UPLOAD(bool, value)
				break;
		case GL_BOOL_VEC2:
			UPLOAD(glm::bvec2, value)
				break;
		case GL_BOOL_VEC3:
			UPLOAD(glm::bvec3, value)
				break;
		case GL_BOOL_VEC4:
			UPLOAD(glm::bvec4, value)
				break;
		case GL_FLOAT_MAT2:
			UPLOAD(glm::mat2, value)
				break;
		case GL_FLOAT_MAT3:
			UPLOAD(glm::mat3, value)
				break;
		case GL_FLOAT_MAT4:
			UPLOAD(glm::mat4, value)
				break;
		case GL_SAMPLER_2D:
			uploadTexture(driverUniforms, textures, value);
		case GL_SAMPLER_CUBE:
			uploadTexture(driverUniforms, textures, value);
		default:
			break;
		}
	}

	void SingleUniform::uploadTexture(
		const std::shared_ptr<DriverUniforms>& driverUniforms,
		const DriverTextures::Ptr& textures,
		const std::any& value
	)
	{

	}






}