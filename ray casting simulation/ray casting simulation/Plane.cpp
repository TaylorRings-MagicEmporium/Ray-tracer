#include "Plane.h"


Plane::Plane(glm::vec3 Position, glm::vec3 Normal, glm::vec3 diffuseColour, float shininess) {
	this->position = Position;
	this->normal = Normal;

	this->diffuseColour = diffuseColour;
	this->ambientColour = diffuseColour;
	this->Shininess = shininess;

	this->Smallest = position - glm::vec3(1e06);
	this->Biggest = position + glm::vec3(1e06);

}

bool Plane::IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out) {
	
	float RPD = glm::dot(normal,RayDir);

	if (glm::abs(RPD) > ep) {
		glm::vec3 dist = position - RayOrigin;
		float t = glm::dot(dist, normal) / RPD;

		out.distance = t;
		out.normal = normal;
		out.intersectionPoint = RayOrigin + t * RayDir;
		out.shape = this;

		return (t >= 0);
	}
	return false;



}