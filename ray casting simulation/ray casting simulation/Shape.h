#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
class Shape
{
public:
	glm::vec3 position;
	glm::vec3 SurColour;
	virtual bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t) = 0;
};

