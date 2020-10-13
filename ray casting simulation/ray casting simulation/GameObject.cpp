#include "GameObject.h"

GameObject::GameObject(glm::vec3 position) {
	//loadOBJ(path, vertices, normals);
 //   this->diffuseColour = diffuseColour;
 //   this->shininess = shininess;
    this->position = position;
    BB = BoundingBox();
}

void GameObject::AddMesh(const char* path, glm::vec3 diffuseColour, float shininess) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    loadOBJ(path, vertices, normals);

    glm::vec3 smallest = glm::vec3(FLT_MAX);
    glm::vec3 biggest = glm::vec3(-FLT_MAX);

    for (int i = 0; i < vertices.size(); i += 3) {
        ShapeList.push_back(new Triangle(vertices[i] + position, vertices[i + 1] + position, vertices[i + 2] + position, normals[i], normals[i + 1], normals[i + 2], diffuseColour, shininess));
    }

    for (int i = 0; i < ShapeList.size(); i++) {
        smallest = glm::min(smallest, ShapeList[i]->Smallest);
        biggest = glm::max(biggest, ShapeList[i]->Biggest);
    }

    BB.AdjustBox(smallest, biggest);

    std::cout << "added mesh triangles!" << std::endl;
}

void GameObject::AddShape(Shape* sh) {
    ShapeList.push_back(sh);
    BB.AdjustBox(position + sh->Smallest, position + sh->Biggest);
}