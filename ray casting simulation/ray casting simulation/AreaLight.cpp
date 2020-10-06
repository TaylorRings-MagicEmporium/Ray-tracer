#include "AreaLight.h"


AreaLight::AreaLight(glm::vec3 position, glm::vec3 intensity, glm::vec3 axisSize, glm::vec3 gridSize) {
	this->position = position;
	this->intensity = intensity;
	this->axisSize = axisSize;

	this->gridSize = gridSize;

	GeneratePoints();
}

void AreaLight::GeneratePoints() {
	for (int x = 0; x < gridSize.x; x++) {
		for (int y = 0; y < gridSize.y; y++) {
			for (int z = 0; z < gridSize.z; z++) {
				GridPositions.push_back(glm::vec3((float)x * (axisSize.x / gridSize.x), (float)y * (axisSize.y / gridSize.y), (float)z * (axisSize.z / gridSize.z)) + position);
			}
			
		}
	}
}