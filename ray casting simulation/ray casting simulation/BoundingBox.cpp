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

	//glm::vec3 invdir = 1.0f / rayDir;
	//if (invdir.x >= 0) {
	//	tMinX = (boxMin.x - rayOri.x) / rayDir.x;
	//	tMaxX = (boxMax.x - rayOri.x) / rayDir.x;
	//}
	//else {
	//	tMinX = (boxMax.x - rayOri.x) / rayDir.x;
	//	tMaxX = (boxMin.x - rayOri.x) / rayDir.x;
	//}

	//if (invdir.y >= 0) {
	//	tMinY = (boxMin.y - rayOri.y) / rayDir.y;
	//	tMaxY = (boxMax.y - rayOri.y) / rayDir.y;
	//}

	//else {
	//	tMinX = (boxMax.x - rayOri.x) / rayDir.x;
	//	tMaxX = (boxMin.x - rayOri.x) / rayDir.x;
	//}
	//if (invdir.z >= 0) {
	//	tMinX = (boxMin.z - rayOri.z) / rayDir.z;
	//	tMaxX = (boxMax.z - rayOri.z) / rayDir.z;
	//}
	//else {
	//	tMinX = (boxMin.z - rayOri.z) / rayDir.z;
	//	tMaxX = (boxMax.z - rayOri.z) / rayDir.z;
	//}


	//tMin = tMinX; // the smallest and largest value from all six faces in the x and y direction
	//tMax = tMaxX;

	//if (tMinX > tMaxY || tMinY > tMaxX) return false; // if the ray is beyond the range of the box by the value of x and y, then it is not intersecting.

	//if (tMinY > tMin) tMin = tMinY; // confines the box by finding the biggest and smallest values of x and y.
	//if (tMaxY < tMax) tMax = tMaxY; // this makes sure that instead of doing three seperate statements, we can just find the smallest/ biggest of 2 then calculate the last.

	//if (tMin > tMinZ || tMinZ > tMax) return false;

	//if (tMinZ > tMin) tMin = tMinZ; // if the min value of z is larger than the min of x and y, then tMin is updated to the larger value. (larger meaning further away from the box)
	//if (tMaxZ < tMax) tMax = tMaxZ; // .

	//return true;

	//glm::vec3 planeLimit[6][2];



	float t[6][3][2]; // each plane, for each axis, for each inersection
	float rayX = rayOri.x / rayDir.x;
	float rayY = rayOri.y / rayDir.y;
	float rayZ = rayOri.z / rayDir.z;
	for (int i = 0; i < 6; i++) {
		t[i][0][0] = planeLimit[i][0].x - rayX;
		t[i][0][1] = planeLimit[i][1].x - rayX;

		t[i][1][0] = planeLimit[i][0].y - rayY;
		t[i][1][1] = planeLimit[i][1].y - rayY;

		t[i][2][0] = planeLimit[i][0].z - rayZ;
		t[i][2][1] = planeLimit[i][1].z - rayZ;

		glm::vec3 intersectionPoints[6];
		intersectionPoints[0] = rayOri + rayDir * t[i][0][0];
		intersectionPoints[1] = rayOri + rayDir * t[i][0][1];
		intersectionPoints[2] = rayOri + rayDir * t[i][1][0];
		intersectionPoints[3] = rayOri + rayDir * t[i][1][1];
		intersectionPoints[4] = rayOri + rayDir * t[i][2][0];
		intersectionPoints[5] = rayOri + rayDir * t[i][2][1];

		for (int b = 0; b < 6; b++) {
			if (intersectionPoints[b].x > planeLimit[i][0].x - 0.001f && intersectionPoints[b].x < planeLimit[i][1].x + 0.001f) {
				if (intersectionPoints[b].y > planeLimit[i][0].y - 0.001f && intersectionPoints[b].y < planeLimit[i][1].y + 0.001f) {
					if (intersectionPoints[b].z > planeLimit[i][0].z - 0.001f && intersectionPoints[b].z < planeLimit[i][1].z + 0.001f) {
						return false;
					}
				}
			}
		}
		return true;
	}

}

void BoundingBox::AdjustBox(glm::vec3 boxMin, glm::vec3 boxMax) {
		this->boxMin = glm::min(this->boxMin, boxMin);
		this->boxMax = glm::max(this->boxMax, boxMax);
}

void BoundingBox::UpdatePlaneLimits() {
	planeLimit[0][0] = glm::vec3(boxMin.x, boxMin.y, boxMax.z);
	planeLimit[0][1] = boxMax;

	planeLimit[1][0] = glm::vec3(boxMin.x, boxMax.y, boxMin.z);
	planeLimit[1][1] = boxMax;

	planeLimit[2][0] = glm::vec3(boxMax.x, boxMin.y, boxMin.z);
	planeLimit[2][1] = boxMax;

	planeLimit[3][0] = boxMin;
	planeLimit[3][1] = glm::vec3(boxMax.x, boxMax.y, boxMin.z);

	planeLimit[4][0] = boxMin;
	planeLimit[4][1] = glm::vec3(boxMax.x, boxMin.y, boxMax.z);

	planeLimit[5][0] = boxMin;
	planeLimit[5][1] = glm::vec3(boxMin.x, boxMax.y, boxMax.z);
}
