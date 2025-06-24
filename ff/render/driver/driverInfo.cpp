#include "driverInfo.h"

namespace ff
{
	DriverInfo::DriverInfo() noexcept
	{

	}
	DriverInfo::~DriverInfo() noexcept
	{

	}


	void DriverInfo::update(uint32_t count, uint32_t glMode, uint32_t instanceCount) noexcept
	{
		m_render.m_calls++;

		switch (glMode)
		{
		case GL_TRIANGLES:
			m_render.m_triangles++;
			break;
		default:
			break;
		}
	}

	void DriverInfo::reset() noexcept
	{
		m_render.m_frame++;
		m_render.m_calls = 0;
		m_render.m_triangles = 0;
	}
}