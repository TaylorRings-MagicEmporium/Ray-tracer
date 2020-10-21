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

	glm::vec3 NormalA;
	glm::vec3 NormalB;
	glm::vec3 NormalC;
	float TriArea;


	// A Triangle is a finite shape using 3 points and barycentric coordinates. because of this, the intersection code is more expensive than the other shapes.
	Triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 normalA, glm::vec3 normalB, glm::vec3 normalC, glm::vec3 diffuseColour, float shininess);
	bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out);
};

