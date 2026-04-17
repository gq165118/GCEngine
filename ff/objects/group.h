#include "../core/object3D.h"

namespace ff
{
	class Group : public Object3D
	{
	public:
		using Ptr = std::shared_ptr<Object3D>;
		static Ptr create()
		{
			return std::make_shared<Group>();
		}

		Group() noexcept
		{
			m_isGroup = true;
		}

		~Group() noexcept
		{

		}

		uint32_t m_groupOrder{ 0 };

	};
}