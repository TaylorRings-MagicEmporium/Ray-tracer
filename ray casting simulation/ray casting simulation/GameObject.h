#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "objloader.h"
#include "Triangle.h"
class GameObject
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	glm::vec3 position;
	glm::vec3 diffuseColour;
	float shininess;

public:
	GameObject(const char* path, glm::vec3 position, glm::vec3 diffuseColour, float shininess);

	void AddMesh(std::vector<Shape*>& shapeList);
};

