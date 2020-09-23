#pragma once
#include "Shape.h"
class Sphere : public Shape
{
public:
	float radius;

	Sphere(glm::vec3 position, float radius, glm::vec3 diffuseColour,float shininess = 0);
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out);
};

