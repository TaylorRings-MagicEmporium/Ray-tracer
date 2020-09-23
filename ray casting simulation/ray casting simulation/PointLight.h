#pragma once
#include <glm/glm.hpp>
class PointLight
{
public:
	glm::vec3 position;
	glm::vec3 intensity;

	PointLight(glm::vec3 position, glm::vec3 intensity);

};

