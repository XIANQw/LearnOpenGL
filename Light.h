#pragma once

#include <glm/glm.hpp>

#include "GameObj.h"

namespace myLight {

	struct {
		const glm::vec3 KA = glm::vec3(0.1f);
		const glm::vec3 KD = glm::vec3(0.5f);
		const glm::vec3 KS = glm::vec3(1.0f);
		const glm::vec3 COLOR = glm::vec3(1.0f);

		/*
		点光源衰减公式的常数项, 一次项和二次项, 这里取点光源覆盖半径为50米时的各项值
		ref: https://learnopengl-cn.github.io/02%20Lighting/05%20Light%20casters/#_4
		*/
		const float Kc = 1.0, Kl = 0.09, Kq = 0.032;
		const float cutOff = glm::cos(glm::radians(12.5f)), outerCutOff = glm::cos(glm::radians(17.5f));
	}DEFAULT;

	struct Light {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;

		std::unique_ptr<GameObj> p_obj;
		Light() :ambient(DEFAULT.KA), diffuse(DEFAULT.KD), specular(DEFAULT.KS), color(DEFAULT.COLOR) {}
	};

	struct DirLight : public Light {
		glm::vec3 direction;
		DirLight() : direction(glm::vec3(0)) {}
	};

	struct PointLight : public Light {
		glm::vec3 position;
		float Kc, Kl, Kq;
		PointLight() : position(glm::vec3(0)), Kc(DEFAULT.Kc), Kl(DEFAULT.Kl), Kq(DEFAULT.Kq){}
		PointLight(glm::vec3 pos) : position(pos), Kc(DEFAULT.Kc), Kl(DEFAULT.Kl), Kq(DEFAULT.Kq) {}
		PointLight(const PointLight& light) = default;
	};

	struct SpotLight : public Light {
		glm::vec3 position;
		glm::vec3 direction;
		float Kc, Kl, Kq, cutOff, outerCutOff;
		SpotLight() : position(glm::vec3(0)), direction(glm::vec3(0)),
			Kc(DEFAULT.Kc), Kl(DEFAULT.Kl), Kq(DEFAULT.Kq),
			cutOff(DEFAULT.cutOff), outerCutOff(DEFAULT.outerCutOff) {}
	};


}