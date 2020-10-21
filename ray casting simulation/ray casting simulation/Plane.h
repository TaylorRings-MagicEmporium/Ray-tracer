#pragma once
#include "Shape.h"
class Plane : public Shape
{
public:
	// a plane is an infinite 3D shape which its rotation is based off the normal of the plane
	Plane(glm::vec3 position, glm::vec3 Normal, glm::vec3 diffuseColour, float shininess);
	glm::vec3 normal;
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out);
};

