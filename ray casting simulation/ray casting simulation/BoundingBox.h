#pragma once
#include <glm/glm.hpp>
class BoundingBox
{
	glm::vec3 boxMin;
	glm::vec3 boxMax;

public:
	BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax);
	bool IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir);
};

