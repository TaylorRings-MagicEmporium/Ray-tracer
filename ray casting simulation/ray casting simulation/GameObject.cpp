#include "GameObject.h"

GameObject::GameObject(const char* path, glm::vec3 position, glm::vec3 diffuseColour, float shininess) {
	loadOBJ(path, vertices, normals);
    this->diffuseColour = diffuseColour;
    this->shininess = shininess;
    this->position = position;
}

void GameObject::AddMesh(std::vector<Shape*>& shapeList) {
    for (int i = 0; i < vertices.size(); i += 3) {
        shapeList.push_back(new Triangle(vertices[i] + position, vertices[i + 1] + position, vertices[i + 2] + position, normals[i], normals[i + 1], normals[i + 2], diffuseColour, shininess));
    }

    std::cout << "added mesh triangles!" << std::endl;
}