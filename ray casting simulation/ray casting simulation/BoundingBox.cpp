#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 BoxMin, glm::vec3 BoxMax) {
	this->boxMin = BoxMin;
	this->boxMax = BoxMax;
}

bool BoundingBox::IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir) {

	float t = glm::dot(rayDir, boxMin);

	glm::vec3 ip = rayOri + t * rayDir;

	if (ip.x > boxMin.x && ip.x < boxMax.x) {
		if (ip.y > boxMin.y && ip.y < boxMax.y) {
			if (ip.z > boxMin.z && ip.z < boxMax.z) {
				return true;
			}
		}
	}
	return false;
}
