#pragma once
#include <glm/glm.hpp>
#include "Shape.h"


class Shape;

// Hitinfo is used to contain various values like distance and hit point without it being stored in the shape itself.
struct HitInfo {

	HitInfo() {};
	float distance;
	glm::vec3 intersectionPoint;
	glm::vec3 normal;
	Shape* shape;
};