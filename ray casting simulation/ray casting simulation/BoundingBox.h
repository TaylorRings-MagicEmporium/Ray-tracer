#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shape.h"
class BoundingBox
{




public:

	//a mathematical finite cuboid that has very cheap intersection code and is used used to reduce the list of shapes
	BoundingBox();

	//IntersectTest is used to test whether a ray has intersected a box or not
	bool IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir, std::vector<Shape*>& returnedList);

	// used to give a box a minimum and maximum point to define it's size
	void AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax);
	
	std::vector<BoundingBox> children;
	std::vector<Shape*> ContainedShapes;

	glm::vec3 boxMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 boxMax = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
};

