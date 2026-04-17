#include "driverMaterials.h"
#include "../../textures/texture.h"

namespace ff {

	DriverMaterial::DriverMaterial() noexcept {}

	DriverMaterial::~DriverMaterial() noexcept {}

	DriverMaterials::DriverMaterials(const DriverPrograms::Ptr& programs) noexcept {
		mPrograms = programs;
		EventDispatcher::getInstance()->addEventListener("materialDispose", this, &DriverMaterials::onMaterialDispose);
	}

	DriverMaterials::~DriverMaterials() noexcept {
		EventDispatcher::getInstance()->removeEventListener("materialDispose", this, &DriverMaterials::onMaterialDispose);
	}

	DriverMaterial::Ptr DriverMaterials::get(const Material::Ptr& material) noexcept {
		auto iter = mMaterials.find(material->getID());

		if (iter == mMaterials.end()) {
			iter = mMaterials.insert(std::make_pair(material->getID(), DriverMaterial::create())).first;
		}

		return iter->second;
	}

	void DriverMaterials::onMaterialDispose(const EventBase::Ptr& event) {
		auto material = (Material*)event->mTarget;

		//比如我们生成了一个material但是并没有使用，然后就析构了
		auto iter = mMaterials.find(material->getID());
		if (iter == mMaterials.end()) {
			return;
		}

		auto dMaterial = iter->second;

		//拿到当前material曾经使用过的所有DriverPrograms, 并且放弃掉对他们的引用计数
		auto programs = dMaterial->mPrograms;
		for (const auto& pIter : programs) {
			auto program = pIter.second;

			//program是DriverProgram
			//mPrograms是DriverPrograms
			mPrograms->release(program);
		}

		mMaterials.erase(iter);
	}

	void DriverMaterials::refreshMaterialUniforms(UniformHandleMap& uniformHandleMap, const Material::Ptr& material) {
		uniformHandleMap["opacity"].mValue = material->m_opacity;
		uniformHandleMap["opacity"].mNeedsUpdate = true;

		if (material->m_isMeshBasicMaterial) {
			auto basicMaterial = std::static_pointer_cast<MeshBasicMaterial>(material);
			refreshMaterialBasic(uniformHandleMap, basicMaterial);
		}

		if (material->m_isMeshPhongMaterial) {
			auto phongMaterial = std::static_pointer_cast<MeshPhongMaterial>(material);
			refreshMaterialPhong(uniformHandleMap, phongMaterial);
		}

		if (material->m_isCubeMaterial) {
			auto cubeMaterial = std::static_pointer_cast<CubeMaterial>(material);
			refreshMaterialCube(uniformHandleMap, cubeMaterial);
		}
	}

	void DriverMaterials::refreshMaterialPhong(UniformHandleMap& uniformHandleMap, const MeshPhongMaterial::Ptr& material) {
		uniformHandleMap["shininess"].mValue = material->mShininess;
		uniformHandleMap["shininess"].mNeedsUpdate = true;

		if ((material->m_diffuseMap && material->m_diffuseMap->m_needUpdate) || material->m_needUpdate) {
			uniformHandleMap["diffuseMap"].mValue = material->m_diffuseMap;
			uniformHandleMap["diffuseMap"].mNeedsUpdate = true;
		}

		if ((material->m_normalMap && material->m_normalMap->m_needUpdate) || material->m_needUpdate) {
			uniformHandleMap["normalMap"].mValue = material->m_normalMap;
			uniformHandleMap["normalMap"].mNeedsUpdate = true;
		}

		if ((material->m_specularMap && material->m_specularMap->m_needUpdate) || material->m_needUpdate) {
			uniformHandleMap["specularMap"].mValue = material->m_specularMap;
			uniformHandleMap["specularMap"].mNeedsUpdate = true;
		}
	}

	void DriverMaterials::refreshMaterialBasic(UniformHandleMap& uniformHandleMap, const MeshBasicMaterial::Ptr& material) {
		if ((material->m_diffuseMap && material->m_diffuseMap->m_needUpdate) || material->m_needUpdate) {
			uniformHandleMap["diffuseMap"].mValue = material->m_diffuseMap;
			uniformHandleMap["diffuseMap"].mNeedsUpdate = true;
		}
	}

	void DriverMaterials::refreshMaterialCube(UniformHandleMap& uniformHandleMap, const CubeMaterial::Ptr& material) {
		if ((material->m_envMap && material->m_envMap->m_needUpdate) || material->m_needUpdate) {
			uniformHandleMap["envMap"].mValue = material->m_envMap;
			uniformHandleMap["envMap"].mNeedsUpdate = true;
		}
	}

}