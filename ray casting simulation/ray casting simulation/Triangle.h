#pragma once
#include "Shape.h"
class Triangle : public Shape
{
public:

	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t);
};

