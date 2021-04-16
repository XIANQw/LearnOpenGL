#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Triangle {
private:
	glm::vec3 v0, v1, v2;
	glm::vec2 uv0, uv1, uv2;


public:
	Triangle() = default;
	Triangle(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2):v0(v0),v1(v1),v2(v2){}
	
	void setVertex(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
		this->v0 = v0;
		this->v1 = v1;
		this->v2 = v2;
	}

	void setUVs(glm::vec2& uv0, glm::vec2& uv1, glm::vec2& uv2) {
		this->uv0 = uv0;
		this->uv1 = uv1;
		this->uv2 = uv2;
	}

	std::vector<float> getVertex() {
		std::vector<float> vertex{
			v0.x, v0.y, v0.z,
			v1.x, v1.y, v1.z,
			v2.x, v2.y, v2.z
		};
		return vertex;
	}

	std::vector<float> getVertexUVCoords() {
		std::vector<float> vertex{
			v0.x, v0.y, v0.z, uv0.x, uv0.y,
			v1.x, v1.y, v1.z, uv1.x, uv1.y,
			v2.x, v2.y, v2.z, uv2.x, uv2.y
		};
		return vertex;
	}

	std::vector<uint32_t> getIndices() {
		return std::vector<uint32_t>{0, 1, 2};
	}
	
	Triangle static defaultTriangle() {
		Triangle t;
		glm::vec3 v0( -0.5f, -0.5f, 0.0f);
		glm::vec3 v1( 0.5f, -0.5f, 0.0f );
		glm::vec3 v2( 0.0f,  0.5f, 0.0f );
		glm::vec2 uv0( 0.0f, 0.0f ); // 左下角
		glm::vec2 uv1( 1.0f, 0.0f ); // 右下角
		glm::vec2 uv2( 0.5f, 1.0f ); // 上中
		t.setVertex(v0, v1, v2);
		t.setUVs(uv0, uv1, uv2);
		return t;
	}
};