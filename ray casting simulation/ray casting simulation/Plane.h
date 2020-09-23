#pragma once
#include "Shape.h"
class Plane : public Shape
{
public:
	Plane(glm::vec3 position, glm::vec3 Normal, glm::vec3 diffuseColour, float shininess);
	glm::vec3 normal;
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out);
};

