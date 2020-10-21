#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <vector>
#include <iostream>
#include "Light.h"
class AreaLight : public Light
{
public:
	glm::vec3 axisSize;
	glm::vec3 gridSize;

	glm::vec3 middleBox;

	std::vector<glm::vec3> GridPositions;

	// An arealight is a finite area of which light comes out. this is calculated as a range of sample points in a grid-like form.
	AreaLight(glm::vec3 position, glm::vec3 intensity, glm::vec3 axisSize ,glm::vec3 gridSize);
	
	// Generates the points needed to calculate soft shadows
	void GeneratePoints();

	// If the sample rate has been changed, then the light is updated and creates a new set of light points
	void ChangeSampleRate(int sampleRate);

	// to reduce the banding effect of the shadows, GetNoise is will calculate a random offset for each light ray
	glm::vec3 GetNoise(int randVal, int max);

};

