#pragma once
#include "Shape.h"
class Triangle : public Shape
{
public:
	//glm::vec2 BaryCoords;
	glm::vec3 pointA;
	glm::vec3 pointB;
	glm::vec3 pointC;

	glm::vec3 Normal;
	float TriArea;



	Triangle(glm::vec3 SurColour, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC);
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, float& t);
};

