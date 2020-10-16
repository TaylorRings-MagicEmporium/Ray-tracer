#include "AreaLight.h"


AreaLight::AreaLight(glm::vec3 position, glm::vec3 intensity, glm::vec3 axisSize, glm::vec3 gridSize) {
	this->position = position;
	this->intensity = intensity;
	this->axisSize = axisSize;

	this->gridSize = gridSize;

	GeneratePoints();
}

void AreaLight::GeneratePoints() {
	if (!GridPositions.empty()) {
		GridPositions.clear();
	}
	float middleX = (axisSize.x / gridSize.x) * 0.5;
	float middleY = (axisSize.y / gridSize.y) * 0.5;
	float middleZ = (axisSize.z / gridSize.z) * 0.5;
	glm::vec3 middleBox = glm::vec3(middleX, middleY, middleZ);
	//float RandVal;
	glm::vec3 randPos;


	for (int x = 0; x < gridSize.x; x++) {
		for (int y = 0; y < gridSize.y; y++) {
			for (int z = 0; z < gridSize.z; z++) {
				//srand(rand());
				float RandValX = (float)rand() / (float)RAND_MAX;
				float RandValY = (float)rand() / (float)RAND_MAX;
				float RandValZ = (float)rand() / (float)RAND_MAX;

				randPos = glm::vec3(glm::lerp(-middleX, middleX, RandValX), glm::lerp(-middleY, middleY, RandValY), glm::lerp(-middleZ, middleZ, RandValZ));
				//std::cout << randPos.x << ", " << randPos.y << ", " << randPos.z << std::endl;
				GridPositions.push_back(glm::vec3((float)x * (axisSize.x / gridSize.x), (float)y * (axisSize.y / gridSize.y), (float)z * (axisSize.z / gridSize.z)) + 
					position + 
					middleBox + 
					randPos);
			}
			
		}
	}
}

void AreaLight::ChangeSampleRate(int sampleRate) {
	gridSize = glm::vec3(sampleRate, sampleRate, sampleRate);
	GeneratePoints();
}