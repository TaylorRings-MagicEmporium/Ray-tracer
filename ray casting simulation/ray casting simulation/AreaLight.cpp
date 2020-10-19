#include "AreaLight.h"


AreaLight::AreaLight(glm::vec3 position, glm::vec3 intensity, glm::vec3 axisSize, glm::vec3 gridSize) {
	this->position = position;
	this->intensity = intensity;
	this->axisSize = axisSize;

	this->gridSize = gridSize;

	float middleX = (axisSize.x / gridSize.x) * 0.5;
	float middleY = (axisSize.y / gridSize.y) * 0.5;
	float middleZ = (axisSize.z / gridSize.z) * 0.5;
	middleBox = glm::vec3(middleX, middleY, middleZ);

	GeneratePoints();
}

void AreaLight::GeneratePoints() {
	if (!GridPositions.empty()) {
		GridPositions.clear();
	}

	//float RandVal;
	glm::vec3 randPos;


	for (int x = 0; x < gridSize.x; x++) {
		for (int y = 0; y < gridSize.y; y++) {
			for (int z = 0; z < gridSize.z; z++) {
				GridPositions.push_back(glm::vec3((float)x * (axisSize.x / gridSize.x), (float)y * (axisSize.y / gridSize.y), (float)z * (axisSize.z / gridSize.z)) + 
					position + 
					middleBox);
			}
			
		}
	}
}

glm::vec3 AreaLight::GetNoise(int randVal, int max) {
	float RandValX = (float)randVal / (float)max;
	float RandValY = (float)randVal / (float)max;
	float RandValZ = (float)randVal / (float)max;

	return glm::vec3(glm::lerp(-middleBox.x, middleBox.x, RandValX), glm::lerp(-middleBox.y, middleBox.y, RandValY), glm::lerp(-middleBox.z, middleBox.z, RandValZ));

}

void AreaLight::ChangeSampleRate(int sampleRate) {
	if (sampleRate == 0) {
		gridSize = glm::vec3(0, 0, 0);
	}
	else {
		gridSize = glm::vec3(sampleRate, sampleRate, 1);

	}
	gridSize = glm::vec3(sampleRate, sampleRate, 1);
	float middleX = (axisSize.x / gridSize.x) * 0.5;
	float middleY = (axisSize.y / gridSize.y) * 0.5;
	float middleZ = (axisSize.z / gridSize.z) * 0.5;
	middleBox = glm::vec3(middleX, middleY, middleZ);
	GeneratePoints();
}