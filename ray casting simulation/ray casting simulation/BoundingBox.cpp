#include "BoundingBox.h"
//#include <math.h>
BoundingBox::BoundingBox() {

}

bool BoundingBox::IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir, std::vector<Shape*>& returnedList) {

	float tmin = (boxMin.x - rayOri.x) / rayDir.x;
	float tmax = (boxMax.x - rayOri.x) / rayDir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (boxMin.y - rayOri.y) / rayDir.y;
	float tymax = (boxMax.y - rayOri.y) / rayDir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (boxMin.z - rayOri.z) / rayDir.z;
	float tzmax = (boxMax.z - rayOri.z) / rayDir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (this->children.empty()) {
		returnedList.insert(returnedList.end(), ContainedShapes.begin(), ContainedShapes.end());
	}
	else {

		for (int i = 0; i < this->children.size(); i++) {
			this->children[i].IntersectTest(rayOri, rayDir, returnedList);
		}
	}

	return true;

}

void BoundingBox::AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax) {
		this->boxMin = glm::min(this->boxMin, boxMin);
		this->boxMax = glm::max(this->boxMax, boxMax);
}
