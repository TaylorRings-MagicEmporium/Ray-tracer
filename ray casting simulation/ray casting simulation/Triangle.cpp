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

	//float u;
	//float v;
	////step 1: finding p
	//// is ray plane parallel?
	//float NDR = glm::dot(Normal, RayDir); // the direction value between normal and ray direction.
	//if (glm::abs(NDR) < ep) return false; // the ray is at parallel with the triangle so we cannot see it.


	////compute d
	//float d = glm::dot(Normal, pointA); // calculate 

	////compute t
	//float t = (glm::dot(Normal, RayOrigin) + d) / NDR;
	//if (t < 0) return false;

	////compute intersect point
	//glm::vec3 point = RayOrigin + t * RayDir;

	////step 2 inside-outside
	//glm::vec3 C;
	//glm::vec3 CheckEdge;
	//glm::vec3 PTV;
	//CheckEdge = pointB - pointA;
	//PTV = point - pointA;
	//C = glm::cross(CheckEdge, PTV);
	//if (glm::dot(Normal, C) < 0) return false; //outside

	//CheckEdge = pointC - pointB;
	//PTV = point - pointB;
	//C = glm::cross(CheckEdge, PTV);
	//if (u = glm::dot(Normal, C) < 0) return false; //outside

	//CheckEdge = pointA - pointC;
	//PTV = point - pointC;
	//C = glm::cross(CheckEdge, PTV);
	//if (v = glm::dot(Normal, C) < 0) return false; //outside


	//u /= TriArea;
	//v /= TriArea;

	//out.distance = t;
	//out.intersectionPoint = point;
	//out.normal = glm::normalize((1.0f - u - v) * NormalA + u * NormalB + v * NormalC);

	//return true;

}