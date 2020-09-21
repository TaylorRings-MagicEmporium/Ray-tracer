// ray casting simulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include "Sphere.h"
#include "Shape.h"
#include "Plane.h"
#include "Triangle.h"

#define PI 3.14159265

int main()
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    Shape* ShapeList[6] = {
       new Sphere(glm::vec3(0,0,-20),glm::vec3(1.0,0.32,0.36),4),
       new Sphere(glm::vec3(5,-1,-15),glm::vec3(0.9,0.76,0.46),2),
       new Sphere(glm::vec3(5,0,-25),glm::vec3(0.65,0.77,0.97),3),
       new Sphere(glm::vec3(-5.5,0,-15),glm::vec3(0.9,0.9,0.9),3),
       ////new Sphere(glm::vec3(0,-10004,-20),glm::vec3(0.2,0.2,0.2),10000)
       new Plane(glm::vec3(0,-10004,-20),glm::vec3(0.2,0.2,0.2),glm::vec3(0,1,0)),
       new Triangle(glm::vec3(0,1,0),glm::vec3(0,1,-2),glm::vec3(-1.9,-1,-2),glm::vec3(1.6,-0.5,-2))
    };

    glm::vec3** image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];
    
    glm::vec3 rayOrigin = glm::vec3(0,0,0);

    float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
    glm::vec2 pixelN, pixelR, pixelC;
    
    float tanValue = glm::tan(glm::radians(60.0f) / 2.0f);
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            
            pixelN = glm::vec2((x + 0.5) / WIDTH, (y + 0.5) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            pixelR = glm::vec2((2 * pixelN.x - 1)*IAR, 1 - 2 * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);

            glm::vec3 PixelColour = glm::vec3(1, 1, 1); // default is white

            float t;
            float smallestT;
            bool first = true;
            for (int i = 0; i < 6; i++){
                if (ShapeList[i]->IntersectTest(rayOrigin, glm::normalize(CamSpace - rayOrigin), t)) { //if true and output is t
                    if (first) { // if it's the first value
                        smallestT = t; //classify it as the smallest
                        first = false;
                        PixelColour = ShapeList[i]->SurColour; // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
                    }
                    else {
                        if (t < smallestT) { // if t is closer to the ray origin than the smallest t
                            smallestT = t; // replace the smallest value and the pixel colour of the now closest hit.
                            PixelColour = ShapeList[i]->SurColour;
                        }
                    }
                }
            }
             //if there is no intersection, it is presumed that there is no collision at all. this means that the final colour is the background colour.
            
            image[x][y] = PixelColour;
            
        }
    }

    // ray direction = cameraSpace - ray origin (camera origin)

    std::ofstream ofs("./testing plane ground.ppm", std::ios::out | std::ios::binary);
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

