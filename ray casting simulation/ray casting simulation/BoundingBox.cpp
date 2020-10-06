#include "BoundingBox.h"
//#include <math.h>
BoundingBox::BoundingBox() {

}

bool BoundingBox::IntersectTest(glm::vec3 rayOri, glm::vec3 rayDir) {

	float tMinX = 0, tMinY = 0, tMinZ = 0, tMaxX = 0, tMaxY = 0, tMaxZ = 0, tMin,tMax;
	//calculating where the ray is at the point of the min and max values of the box.
	// it does this by finding the distance from the origin to the box point, then find where it is on the line by dividing by the unit

	//tMinY = (boxMin.y - rayOri.y) / rayDir.y;
	//tMinZ = (boxMin.z - rayOri.z) / rayDir.z;

	//tMaxY = (boxMax.y - rayOri.y) / rayDir.y;
	//tMaxZ = (boxMax.z - rayOri.z) / rayDir.z;

	glm::vec3 invdir = 1.0f / rayDir;
	if (invdir.x >= 0) {
		tMinX = (boxMin.x - rayOri.x) / rayDir.x;
		tMaxX = (boxMax.x - rayOri.x) / rayDir.x;
	}
	else {
		tMinX = (boxMax.x - rayOri.x) / rayDir.x;
		tMaxX = (boxMin.x - rayOri.x) / rayDir.x;
	}

	if (invdir.y >= 0) {
		tMinY = (boxMin.y - rayOri.y) / rayDir.y;
		tMaxY = (boxMax.y - rayOri.y) / rayDir.y;
	}

	else {
		tMinX = (boxMax.x - rayOri.x) / rayDir.x;
		tMaxX = (boxMin.x - rayOri.x) / rayDir.x;
	}
	if (invdir.z >= 0) {
		tMinX = (boxMin.z - rayOri.z) / rayDir.z;
		tMaxX = (boxMax.z - rayOri.z) / rayDir.z;
	}
	else {
		tMinX = (boxMin.z - rayOri.z) / rayDir.z;
		tMaxX = (boxMax.z - rayOri.z) / rayDir.z;
	}
	//if (tMinX > tMaxX) std::swap(tMinX, tMaxX); // depending on the ray these will differ so the biggest and smallest needs to be discovered first
	//if (tMinY > tMaxY) std::swap(tMinY, tMaxY); // .
	//if (tMinZ > tMaxZ) std::swap(tMinZ, tMaxZ); // .

	tMin = tMinX; // the smallest and largest value from all six faces in the x and y direction
	tMax = tMaxX;

	if (tMinX > tMaxY || tMinY > tMaxX) return false; // if the ray is beyond the range of the box by the value of x and y, then it is not intersecting.

	if (tMinY > tMin) tMin = tMinY; // confines the box by finding the biggest and smallest values of x and y.
	if (tMaxY < tMax) tMax = tMaxY; // this makes sure that instead of doing three seperate statements, we can just find the smallest/ biggest of 2 then calculate the last.

	if (tMin > tMinZ || tMinZ > tMax) return false;

	if (tMinZ > tMin) tMin = tMinZ; // if the min value of z is larger than the min of x and y, then tMin is updated to the larger value. (larger meaning further away from the box)
	if (tMaxZ < tMax) tMax = tMaxZ; // .

	return true;

	//float t = glm::dot(rayDir, boxMin);


	//glm::vec3 ip = rayOri + t * rayDir;

	//if (ip.x > boxMin.x && ip.x < boxMax.x) {
	//	if (ip.y > boxMin.y && ip.y < boxMax.y) {
	//		if (ip.z > boxMin.z && ip.z < boxMax.z) {
	//			return true;
	//		}
	//	}
	//}
	//return false;


}

void BoundingBox::AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax) {
		this->boxMin = glm::min(this->boxMin, boxMin);
		this->boxMax = glm::max(this->boxMax, boxMax);
}
