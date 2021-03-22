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
	}DEFAULT;

	struct Light {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 color;

		std::unique_ptr<GameObj> p_obj;
		Light() :ambient(DEFAULT.KA), diffuse(DEFAULT.KD), specular(DEFAULT.KS), color(DEFAULT.COLOR) {}
	};

	struct PointLight : public Light {
		glm::vec3 position;
		float Kc, Kl, Kq;
		PointLight() : position(glm::vec3(0)), Kc(DEFAULT.Kc), Kl(DEFAULT.Kl), Kq(DEFAULT.Kq){}
		PointLight(glm::vec3 pos) : position(pos), Kc(DEFAULT.Kc), Kl(DEFAULT.Kl), Kq(DEFAULT.Kq) {}
	};


}