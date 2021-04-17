#pragma once
#include <glm/glm.hpp>
#include <vector>


struct Vertex {
	glm::vec3 pos;
	static const std::vector<uint32_t> layout;
	
	Vertex() = default;
	Vertex(glm::vec3 pos) : pos(pos) {}
};
const std::vector<uint32_t> Vertex::layout = { 0, 3 };

struct VertexTex {
	glm::vec3 pos;
	glm::vec2 tex;
	static const std::vector<uint32_t> layout;
	
	VertexTex() = default;
	VertexTex(glm::vec3 pos, glm::vec2 tex) : pos(pos), tex(tex) {}
};
const std::vector<uint32_t> VertexTex::layout = { 0, 3, 5 };


struct VertexNormal {
	glm::vec3 pos;
	glm::vec3 normal;
	static const std::vector<uint32_t> layout;

	VertexNormal() = default;
	VertexNormal(glm::vec3 pos, glm::vec3 normal) : pos(pos), normal(normal) {}
};
const std::vector<uint32_t> VertexNormal::layout = { 0, 3, 6 };

struct VertexNormalColor {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	static const std::vector<uint32_t> layout;

	VertexNormalColor() = default;
	VertexNormalColor(glm::vec3 pos, glm::vec3 normal, glm::vec3 color) : pos(pos), normal(normal), color(color) {}
};
const std::vector<uint32_t> VertexNormalColor::layout = { 0, 3, 6, 9 };


struct VertexNormalTex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texcoord;
	static const std::vector<uint32_t> layout;

	VertexNormalTex() = default;
	VertexNormalTex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texcoord) : pos(pos), normal(normal), texcoord(texcoord) {}
};
const std::vector<uint32_t> VertexNormalTex::layout = { 0, 3, 6, 8 };
