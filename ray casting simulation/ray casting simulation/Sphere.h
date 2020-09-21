#pragma once
#include "Shape.h"
class Sphere : public Shape
{
public:
	//glm::vec3 position;
	float radius;
	//glm::vec3 SurColour;

	Sphere(glm::vec3 position, glm::vec3 SurColour, float radius);
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t);
};

