#include "Plane.h"


Plane::Plane(glm::vec3 Position, glm::vec3 SurColour, glm::vec3 Normal) {
	this->position = Position;
	this->SurColour = SurColour;
	this->normal = Normal;
}

bool Plane::IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t) {

	float RPD = glm::dot(RayDir, normal);

	if (glm::abs(RPD) < ep) return false;
	glm::vec3 dist = position - RayOrigin;
	t = (glm::dot(dist, normal)) / RPD;

	return (t >= 0);
	
}