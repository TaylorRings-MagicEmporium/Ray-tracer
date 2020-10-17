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
        Triangle* t = new Triangle(vertices[i] + position, vertices[i + 1] + position, vertices[i + 2] + position, normals[i], normals[i + 1], normals[i + 2], diffuseColour, shininess);
        BBshapes.push_back(t);
        
    }

    for (int i = 0; i < BBshapes.size(); i++) {
        smallest = glm::min(smallest, BBshapes[i]->Smallest);
        biggest = glm::max(biggest, BBshapes[i]->Biggest);
    }

    BB.AdjustBox(smallest, biggest);
    CreateOctree(BBshapes);
    //BB.ContainedShapes = BBshapes;
    std::cout << "added mesh triangles!" << std::endl;
}

void GameObject::AddShape(Shape* sh) {
    BB.ContainedShapes.push_back(sh);
    BB.AdjustBox(position + sh->Smallest, position + sh->Biggest);
}

void GameObject::CreateOctree(std::vector<Shape*> BBShapeList) {
    glm::vec3 diff = glm::abs((BB.boxMax - BB.boxMin))/2.0f;
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 2; z++) {
                BoundingBox CBB = BoundingBox();

                glm::vec3 min = BB.boxMin + glm::vec3(diff.x * (float)x, diff.y * (float)y, diff.z * (float)z);
                glm::vec3 max = BB.boxMax - glm::vec3(diff.x * (float)(1-x), diff.y * (float)(1-y), diff.z * (float)(1-z));
                CBB.AdjustBox(min, max);
                BB.children.push_back(CBB);
            }
        }
    }
    std::vector<Shape*> what;
    for (int a = 0; a < BBShapeList.size(); a++)
    {
        bool error = true;

        for (int b = 0; b < 8; b++) {

            bool tx = glm::max(BBShapeList[a]->Smallest.x, BB.children[b].boxMin.x) < glm::min(BBShapeList[a]->Biggest.x, BB.children[b].boxMax.x);
            bool ty = glm::max(BBShapeList[a]->Smallest.y, BB.children[b].boxMin.y) < glm::min(BBShapeList[a]->Biggest.y, BB.children[b].boxMax.y);
            bool tz = glm::max(BBShapeList[a]->Smallest.z, BB.children[b].boxMin.z) < glm::min(BBShapeList[a]->Biggest.z, BB.children[b].boxMax.z);

            if (tx && ty && tz) {
                BB.children[b].ContainedShapes.push_back(BBShapeList[a]);
                error = false;
            }

        }
        if (error) {
            what.push_back(BBShapeList[a]);
        }
    }
}

//if ((BBShapeList[a]->Smallest.x) > BB.children[b].boxMin.x && (BBShapeList[a]->Biggest.x) < BB.children[b].boxMax.x) {
//    if ((BBShapeList[a]->Smallest.y) > BB.children[b].boxMin.y && (BBShapeList[a]->Biggest.y) < BB.children[b].boxMax.y) {
//        if ((BBShapeList[a]->Smallest.z) > BB.children[b].boxMin.z && (BBShapeList[a]->Biggest.z) < BB.children[b].boxMax.z) {
//            BB.children[b].ContainedShapes.push_back(BBShapeList[a]);
//            error = false;
//        }
//    }
//}