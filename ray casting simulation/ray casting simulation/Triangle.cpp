#include "Triangle.h"

Triangle::Triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 normalA, glm::vec3 normalB, glm::vec3 normalC, glm::vec3 diffuseColour, float shininess) {
	this->pointA = pointA;
	this->pointB = pointB;
	this->pointC = pointC;

	this->NormalA = normalA;
	this->NormalB = normalB;
	this->NormalC = normalC;

	this->diffuseColour = diffuseColour;
	this->ambientColour = diffuseColour;
	this->Shininess = shininess;

	position = glm::vec3((pointA.x + pointB.x + pointC.x) / 3.0f, (pointA.y + pointB.y + pointC.y) / 3.0f, (pointA.z + pointB.z + pointC.z) / 3.0f);

	glm::vec3 edge1 = pointB - pointA;
	glm::vec3 edge2 = pointC - pointA;

	//don't normalise as the cross can find areas
	//and normal is direction
	Normal = glm::cross(edge1, edge2);
	TriArea = glm::dot(Normal, Normal);

	this->Smallest = glm::min(glm::min(pointA, pointB), pointC);
	this->Biggest = glm::max(glm::max(pointA, pointB), pointC);
}


bool Triangle::IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out) {
	 

	glm::vec3 edge1 = pointB - pointA;
	glm::vec3 edge2 = pointC - pointA;

	glm::vec3 dist = (RayOrigin - pointA);
	glm::vec3 perV = glm::cross(RayDir, edge2);
	float u = glm::dot(dist, perV) / glm::dot(edge1, perV);
	float v = glm::dot(RayDir, glm::cross(dist, edge1)) / glm::dot(edge1, perV);

	if (u < 0 || u > 1) {
		return false;
	}
	else if (v < 0 || u + v > 1) {
		return false;
	}
	else {
		float t = glm::dot(edge2, glm::cross(dist, edge1)) / glm::dot(edge1, perV);
		out.distance = t;
		out.intersectionPoint = (1.0f - u - v) * pointA + u * pointB + v * pointC;
		out.normal = glm::normalize((1.0f - u - v) * NormalA + u * NormalB + v * NormalC);
		return (t >= 0);


	}
	return false;

}