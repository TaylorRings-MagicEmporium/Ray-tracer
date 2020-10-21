#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "objloader.h"
#include "Triangle.h"
#include "BoundingBox.h"
class GameObject
{

	glm::vec3 position;

	//CreateOctree is called in-house and is used to organise mesh's triangles into bounding boxes of various depth.
	void CreateOctree(BoundingBox* BB, int CURRENT_LEVEL, int MAX_LEVEL);
public:
	// a Construct that contains a number of primitves, either as the full shape or the many triangles that a mesh has.
	GameObject(glm::vec3 position);

	// adds a mesh that will be unloaded and be displayed as a group of tris
	void AddMesh(const char* path, glm::vec3 diffuseColour, float shininess);

	// adds a single primitive to the gameobject, making it possible to attach multiple primitives together
	void AddShape(Shape* sh);
	BoundingBox BB;
	std::vector<Shape*> BBshapes;
	bool AvoidBox = false;
	int NoBoxes = 0;



};

