#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "objloader.h"
#include "Triangle.h"
#include "BoundingBox.h"
class GameObject
{

	glm::vec3 position;


public:
	//GameObject(glm::vec3 position, glm::vec3 diffuseColour, float shininess);
	GameObject(glm::vec3 position);
	void AddMesh(const char* path, glm::vec3 diffuseColour, float shininess);

	void AddShape(Shape* sh);
	BoundingBox BB;
	std::vector<Shape*> ShapeList;
};

