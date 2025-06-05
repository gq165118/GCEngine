/**
 * @class Identity
 * @brief Identity单例类，生成ID
 *
 * Example usage:
 * ```
 * ```
 * @note Thread-safe.
 * @author qiang.guo
 * @date 2025-06-04
 */


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