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
#include "GameObject.h"
#include <vector>

#define PI 3.14159265

int main()
{
    const int WIDTH = 640;
    const int HEIGHT = 480;
    float fullRenderPercentage = (float)WIDTH * (float)HEIGHT;
    std::vector<GameObject> objects;



    //objects.push_back(GameObject("OBJ files/teapot_simple_smooth.obj", glm::vec3(0,0,-5), glm::vec3(0.5, 0.5, 0), 100.0f));

    std::vector<Shape*> ShapeList;
    ShapeList.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1.0, 0.32, 0.36), 128)); // red
    ShapeList.push_back(new Sphere(glm::vec3(5, -1, -15), 2,glm::vec3(0.9, 0.76, 0.46), 128)); //green
    ShapeList.push_back(new Sphere(glm::vec3(5, 0, -25), 3,glm::vec3(0.65, 0.77, 0.97), 128)); // blue
    ShapeList.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3,glm::vec3(0.9, 0.9, 0.9), 128)); //cyan
    ShapeList.push_back(new Plane(glm::vec3(0, -4, -20),glm::vec3(0,1,0), glm::vec3(0.7, 0.7, 0.7),0));
    //ShapeList.push_back(new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::normalize(glm::vec3(0.0,0.6,1.0)),glm::vec3(-0.4,-0.4,1.0),glm::vec3(0.4,-0.4,1.0),glm::vec3(0.5,0.5,0.0),100));

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].AddMesh(ShapeList);
    }

    PointLight L = PointLight(glm::vec3(0,20,0), glm::vec3(1.0, 1.0, 1.0));

    glm::vec3** image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];
    
    glm::vec3 rayOrigin = glm::vec3(0,0,0);

    float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
    glm::vec2 pixelN, pixelR, pixelC;
    
    float tanValue = glm::tan(glm::radians(60.0f) / 2.0f);
    int counter = 0;
    for (int y = 0; y < HEIGHT; y++) {
        std::cout << (float)counter * 100.0f / fullRenderPercentage << std::endl;
        for (int x = 0; x < WIDTH; x++) {
            
            pixelN = glm::vec2((x + 0.5) / WIDTH, (y + 0.5) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            pixelR = glm::vec2((2 * pixelN.x - 1)*IAR, 1 - 2 * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);

            glm::vec3 PixelColour = glm::vec3(1, 1, 1); // default is white

            HitInfo dump;
            HitInfo smallestH;
            float smallestT = 1000.0f;
            int closestShape = -1;
            //bool first = true;
            glm::vec3 rayDir = glm::normalize(CamSpace - rayOrigin);
            for (int i = 0; i < ShapeList.size(); i++){
                if (ShapeList[i]->IntersectTest(rayOrigin, rayDir, dump)) { //if true and output is t
                    if (dump.distance < smallestT) {
                        smallestT = dump.distance;
                        smallestH = dump;
                        closestShape = i;//classify it as the smallest
                    }
                        
                }
                

            }
            if (closestShape != -1) {
                glm::vec3 lightRay = L.position - smallestH.intersectionPoint;
                bool pass = false;

                for (int a = 0; a < ShapeList.size(); a++) {
                    if (closestShape != a) {
                        if (ShapeList[a]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * 1e-04f, glm::normalize(lightRay), dump)) {
                            PixelColour = ShapeList[a]->GetAmbientLight();
                            pass = true;
                            break;
                        }
                    }

                }
                if (!pass) {
                    PixelColour = ShapeList[closestShape]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
                    PixelColour += ShapeList[closestShape]->GetDiffuseLight(L, glm::normalize(lightRay), smallestH.normal);
                    PixelColour += ShapeList[closestShape]->GetSpecularLight(L, glm::normalize(lightRay), smallestH.normal, rayDir);
                    
                
                }
            } 
             //if there is no intersection, it is presumed that there is no collision at all. this means that the final colour is the background colour.
            
            image[x][y] = PixelColour;
            
            counter++;
        }
    }

    // ray direction = cameraSpace - ray origin (camera origin)

    std::ofstream ofs("./hard spheres.ppm", std::ios::out | std::ios::binary);
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

