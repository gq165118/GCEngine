#pragma once
#include "directionalLight.h"

namespace ff {

	DirectionalLight::DirectionalLight() noexcept {
		m_isDirectionalLight = true;
		mShadow = DirectionalLightShadow::create();
	}

	DirectionalLight::~DirectionalLight() noexcept {}
}