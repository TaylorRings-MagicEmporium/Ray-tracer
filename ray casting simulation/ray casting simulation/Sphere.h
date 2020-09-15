#pragma once
#include <glm/glm.hpp>
class Sphere
{
public:
	glm::vec3 position;
	float radius;
	glm::vec3 SurColour;

	Sphere(glm::vec3 position, float radius, glm::vec3 SurColour);
};

