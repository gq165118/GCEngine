#include "bone.h"
#include "../log/debugLog.h"

namespace ff {

	Bone::Bone() noexcept {
		m_isBone = true;
	}

	Bone::~Bone() noexcept {}

	glm::mat4 Bone::updateWorldMatrix(bool updateParent, bool updateChildren) noexcept {
		if (!m_parent.expired() && updateParent) {
			auto parent = m_parent.lock();
			parent->updateWorldMatrix(true, false);
		}

		updateMatrix();

		m_worldMatrix = m_localMatrix;

		if (!m_parent.expired()) {
			auto parent = m_parent.lock();

			//骨骼动画智能收到自己的系统影响，
			if (parent->m_isBone) {
				m_worldMatrix = parent->getWorldMatrix() * m_worldMatrix;
			}
		}
 
		//没有动画，就启用Nodematrix
		if (m_localMatrix == glm::mat4(1.0)) {
			m_worldMatrix = m_worldMatrix * mNodeMatrix;
		}

		if (updateChildren) {
			for (auto& child : m_children) {
				child->updateWorldMatrix(false, true);
			}
		}

		return m_worldMatrix;
		
	}
}