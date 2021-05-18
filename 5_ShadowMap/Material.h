#pragma once
#include <glm/glm.hpp>

namespace myMaterial {

	const glm::vec3 DEFAULT_KA = glm::vec3(0.1f);
	const glm::vec3 DEFAULT_KD = glm::vec3(0.5f);
	const glm::vec3 DEFAULT_KS = glm::vec3(0.8f);
	const float DEFAULT_SHININESS = 32.0f;
	const glm::vec4 DEFAULT_COLOR = glm::vec4(1.0f);

	struct Material {
		glm::vec3 Ka;
		glm::vec3 Kd;
		glm::vec3 Ks;
		float shininess;
		glm::vec4 objColorRGBA;
		std::vector<Texture> textures;


		Material() {
			Ka = DEFAULT_KA;
			Kd = DEFAULT_KD;
			Ks = DEFAULT_KS;
			shininess = DEFAULT_SHININESS;
			objColorRGBA = DEFAULT_COLOR;
		}
	};


}