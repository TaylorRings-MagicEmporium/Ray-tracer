// ray casting simulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include "Sphere.h"
#include "Shape.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "HitInfo.h"
#include "PointLight.h"
#include <vector>

#define PI 3.14159265

//void AddMesh(std::vector<glm::vec3>& vertices, std::vector<Shape*>& ShapeList) {
//
//    for (int i = 0; i < vertices.size(); i+= 3) {
//        ShapeList.push_back(new Triangle(glm::vec3((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX), vertices[i] + glm::vec3(-1.5,-1.5,-5), vertices[i+ 1] + glm::vec3(-1.5, -1.5, -5), vertices[i + 2] + glm::vec3(-1.5, -1.5, -5)));
//    }
//
//    std::cout << "added mesh triangles!" << std::endl;
//}

int main()
{
    const int WIDTH = 640;
    const int HEIGHT = 480;
    float fullRenderPercentage = (float)WIDTH * (float)HEIGHT;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;


    //bool res = loadOBJ("OBJ files/teapot_simple.obj", vertices, normals);

    std::vector<Shape*> ShapeList;
    ShapeList.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1.0, 0.32, 0.36), 128));
    //ShapeList.push_back(new Sphere(glm::vec3(5, -1, -15), 2,glm::vec3(0.9, 0.76, 0.46), 128));
    //ShapeList.push_back(new Sphere(glm::vec3(5, 0, -25), 3,glm::vec3(0.65, 0.77, 0.97), 128));
    //ShapeList.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3,glm::vec3(0.9, 0.9, 0.9), 128));
    ShapeList.push_back(new Plane(glm::vec3(-10, -1, -10),glm::vec3(0,1,0), glm::vec3(0.8, 0.8, 0.8),128));
    ShapeList.push_back(new Triangle(glm::vec3(0, 1, -2) + glm::vec3(0,0,0), glm::vec3(-1.9, -1, -2) + glm::vec3(0, 0, 0), glm::vec3(1.6, -0.5, -2) + glm::vec3(0, 0, 0),glm::vec3(0,1,0),0));

    //AddMesh(vertices, ShapeList);

    PointLight L = PointLight(glm::vec3(0, 10, 0), glm::vec3(1.0, 1.0, 1.0));

    glm::vec3** image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];
    
    glm::vec3 rayOrigin = glm::vec3(0,0,0);

    float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
    glm::vec2 pixelN, pixelR, pixelC;
    
    float tanValue = glm::tan(glm::radians(90.0f) / 2.0f);
    int counter = 0;
    for (int y = 0; y < HEIGHT; y++) {
        std::cout << (float)counter * 100.0f / fullRenderPercentage << std::endl;
        for (int x = 0; x < WIDTH; x++) {
            
            pixelN = glm::vec2((x + 0.5) / WIDTH, (y + 0.5) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            pixelR = glm::vec2((2 * pixelN.x - 1)*IAR, 1 - 2 * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);

            glm::vec3 PixelColour = glm::vec3(1, 1, 1); // default is white

            HitInfo h;
            float smallestT;
            bool first = true;
            glm::vec3 rayDir = glm::normalize(CamSpace - rayOrigin);
            for (int i = 0; i < ShapeList.size(); i++){
                if (ShapeList[i]->IntersectTest(rayOrigin, rayDir, h)) { //if true and output is t
                    if (first || h.distance < smallestT) { // if it's the first value
                        smallestT = h.distance; //classify it as the smallest
                        first = false;
                        PixelColour = ShapeList[i]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
                        PixelColour += ShapeList[i]->GetDiffuseLight(L,glm::normalize(L.position - h.intersectionPoint),h.normal);
                        PixelColour += ShapeList[i]->GetSpecularLight(L, glm::normalize(L.position - h.intersectionPoint), h.normal, rayDir);
                    }
                }
            }
             //if there is no intersection, it is presumed that there is no collision at all. this means that the final colour is the background colour.
            
            image[x][y] = PixelColour;
            
            counter++;
        }
    }

    // ray direction = cameraSpace - ray origin (camera origin)

    std::ofstream ofs("./Spec light.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x) {
            ofs << (unsigned char)(glm::min((float)1, (float)image[x][y].x) * 255) <<
                (unsigned char)(glm::min((float)1, (float)image[x][y].y) * 255) <<
                (unsigned char)(glm::min((float)1, (float)image[x][y].z) * 255);
        }
    }
    ofs.close();

    return 0;
}

