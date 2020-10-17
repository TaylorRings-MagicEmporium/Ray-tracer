#pragma once
#include <glm/glm.hpp>
#include "Shape.h"


class Shape;

struct HitInfo {

	HitInfo() {};
	float distance;
	glm::vec3 intersectionPoint;
	glm::vec3 normal;
	Shape* shape;
};