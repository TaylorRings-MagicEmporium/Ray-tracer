#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shape.h"
class BoundingBox
{




public:
	BoundingBox();
	bool IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir, std::vector<Shape*>& returnedList);
	void AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax);
	
	std::vector<BoundingBox> children;
	std::vector<Shape*> ContainedShapes;

	glm::vec3 boxMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 boxMax = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
};

