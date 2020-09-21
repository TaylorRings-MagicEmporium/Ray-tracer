#include "Triangle.h"

Triangle::Triangle(glm::vec3 SurColour, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC) {
	this->SurColour = SurColour;
	this->pointA = pointA;
	this->pointB = pointB;
	this->pointC = pointC;

	position = glm::vec3((pointA.x + pointB.x + pointC.x) / 3.0f, (pointA.y + pointB.y + pointC.y) / 3.0f, (pointA.z + pointB.z + pointC.z) / 3.0f);

	glm::vec3 edge1 = pointB - pointA;
	glm::vec3 edge2 = pointC - pointA;

	//don't normalise as the cross can find areas
	//and normal is direction
	Normal = glm::cross(edge1, edge2);
	TriArea = glm::dot(Normal, Normal);
}


bool Triangle::IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t) {


	float u;
	float v;
	//step 1: finding p
	// is ray plane parallel?
	float NDR = glm::dot(Normal, RayDir); // the direction value between normal and ray direction.
	if (glm::abs(NDR) < ep) return false; // the ray is at parallel with the triangle so we cannot see it.


	//compute d
	float d = glm::dot(Normal, pointA); // calculate 

	//compute t
	t = (glm::dot(Normal, RayOrigin) + d) / NDR;
	if (t < 0) return false;

	//compute intersect point
	glm::vec3 point = RayOrigin + t * RayDir;

	//step 2 inside-outside
	glm::vec3 C;

	glm::vec3 edge3 = pointB - pointA;
	glm::vec3 PTA = point - pointA;
	C = glm::cross(edge3, PTA);
	if (glm::dot(Normal, C) < 0) return false; //outside

	glm::vec3 edge4 = pointC - pointB;
	glm::vec3 PTB = point - pointB;
	C = glm::cross(edge4, PTB);
	if (u = glm::dot(Normal, C) < 0) return false; //outside

	glm::vec3 edge5 = pointA - pointC;
	glm::vec3 PTC = point - pointC;
	C = glm::cross(edge5, PTC);
	if (v = glm::dot(Normal, C) < 0) return false; //outside


	u /= TriArea;
	v /= TriArea;

	return true;

}