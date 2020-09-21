#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
class Shape
{
public:
	glm::vec3 position;
	glm::vec3 SurColour;
	float ep = 0.000001;
	virtual bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t) = 0;
};

