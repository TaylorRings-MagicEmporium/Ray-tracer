#pragma once
#include <glm/glm.hpp>

struct HitInfo {

	HitInfo() {};
	float distance;
	glm::vec3 intersectionPoint;
	glm::vec3 normal;
};