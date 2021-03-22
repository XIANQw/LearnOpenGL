#pragma once

#include <glm/glm.hpp>

#include "GameObj.h"

namespace myLight {

	const glm::vec3 DEFAULT_KA = glm::vec3(0.1f);
	const glm::vec3 DEFAULT_KD = glm::vec3(0.5f);
	const glm::vec3 DEFAULT_KS = glm::vec3(1.0f);
	const glm::vec3 DEFAULT_COLOR = glm::vec3(1.0f);

	struct Light {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;

		std::unique_ptr<GameObj> p_obj;
		Light() :ambient(DEFAULT_KA), diffuse(DEFAULT_KD), specular(DEFAULT_KS), color(DEFAULT_COLOR) {}
	};

	struct PointLight : public Light {
		glm::vec3 position;
	};


}