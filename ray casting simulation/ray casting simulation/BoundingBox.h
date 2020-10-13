#pragma once
#include <glm/glm.hpp>
class BoundingBox
{
	glm::vec3 boxMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 boxMax = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	glm::vec3 planeLimit[6][2];


public:
	BoundingBox();
	bool IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir);
	void AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax);
	void UpdatePlaneLimits();
};

