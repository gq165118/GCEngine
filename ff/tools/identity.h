#pragma once
#include "../global/base.h"

namespace ff
{
	class Identity
	{
	public:
		static ID generateID() { return ++m_currentID; }

	private:
		static ID m_currentID;
	};
}