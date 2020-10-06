#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Light.h"
class AreaLight : public Light
{
public:
	glm::vec3 axisSize;
	glm::vec3 gridSize;

	std::vector<glm::vec3> GridPositions;

	AreaLight(glm::vec3 position, glm::vec3 intensity, glm::vec3 axisSize ,glm::vec3 gridSize);
	void GeneratePoints();

};

