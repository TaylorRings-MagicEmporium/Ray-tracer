#pragma once
#include <glm/glm.hpp>
class BoundingBox
{
	glm::vec3 boxMin = glm::vec3(0.5,0.5,0.5);
	glm::vec3 boxMax = glm::vec3(-0.5,-0.5,-0.5);

public:
	BoundingBox();
	bool IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir);
	void AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax);
};

