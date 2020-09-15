// ray casting simulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include "Sphere.h"

#define PI 3.14159265


bool RaySphereIntersection(Sphere sp, glm::vec3 RayOrigin, glm::vec3 RayDir, float& t) {

    float t0, t1; // two maximum possible points of intersection

    glm::vec3 l = sp.position - RayOrigin; // vector length between ray origin and sphere origin
    float Tca = glm::dot(l, RayDir); // the projection of the vector length onto the direction
    if (Tca < 0) return false; // if it's negative, due to the projection being in the opposite direction of the ray direction, false.

    float s = glm::dot(l, l) - Tca * Tca; // squared magnitude of l and Tca
    if (s > sp.radius * sp.radius) return false; //if the squared length between the centre to right angled to the ray is bigger than the squared radius, then it's outside the sphere

    float Thc = glm::sqrt(sp.radius * sp.radius - s); // vector subtraction to find the length of Thc (from p to centre to middle)
    t0 = Tca - Thc;
    t1 = Tca + Thc;

    if (t0 > t1) std::swap(t0, t1); // smaller value is closer to ray origin than bigger value
    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) {
            return false; // if both values are negative then this is a incorrect ray intersection;
        }
    }

    t = t0; // the first thing we will render is the closest to the camera (the bigger value cannot be seen if opaque)
    return true;
}

int main()
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    Sphere sphereList[5] = {
        Sphere(glm::vec3(0,0,-20),4,glm::vec3(1.0,0.32,0.36)),
        Sphere(glm::vec3(5,-1,-15),2,glm::vec3(0.9,0.76,0.46)),
        Sphere(glm::vec3(5,0,-25),3,glm::vec3(0.65,0.77,0.97)),
        Sphere(glm::vec3(-5.5,0,-15),3,glm::vec3(0.9,0.9,0.9)),
        Sphere(glm::vec3(0,-10004,-20),10000,glm::vec3(0.2,0.2,0.2))
    };

    glm::vec3** image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];
    
    glm::vec3 rayOrigin = glm::vec3(0);

    float IAR = (float)WIDTH / (float)HEIGHT;
    glm::vec2 pixelN, pixelR, pixelC;
    //float a = 1 / 2 * PI; // should be 
    //float tanValue = glm::tan(a / 0.5f);
    //float tanValue = tanf(15.0 * PI / 180.0);
    
    float tanValue = glm::tan(glm::radians(30.0f) / 2.0f);
    
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
            for (int i = 0; i < 5; i++){
                if (RaySphereIntersection(sphereList[i], rayOrigin, glm::normalize(CamSpace - rayOrigin), t)) {
                    if (first) {
                        smallestT = t;
                        first = false;
                        PixelColour = sphereList[i].SurColour;
                    }
                    else {
                        if (t < smallestT) {
                            smallestT = t;
                            PixelColour = sphereList[i].SurColour;
                        }
                    }
                }
            }

            image[x][y] = PixelColour;
            
        }
    }

    // ray direction = cameraSpace - ray origin (camera origin)

    std::ofstream ofs("./ray_Casting_Spheres.ppm", std::ios::out | std::ios::binary);
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

