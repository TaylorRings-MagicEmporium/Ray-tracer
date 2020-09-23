#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 intensity) {
	this->position = position;
	this->intensity = intensity;
}