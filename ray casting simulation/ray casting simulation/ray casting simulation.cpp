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
#include "AreaLight.h"
#include <vector>


#include <SDL2/SDL.h>
#undef main

#define PI 3.14159265
const int WIDTH = 640;
const int HEIGHT = 480;

SDL_Event event;

bool InitSDL(SDL_Window*& window, SDL_Surface*& screenSurface) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_ERROR: " << SDL_GetError() << std::endl;
    }
    else {
        //create the window
        window = SDL_CreateWindow("SDL Version", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            std::cout << "SDL Window could not be created! SDL_ERROR: " << SDL_GetError() << std::endl;
            return false;
        }
        else {
            screenSurface = SDL_GetWindowSurface(window);
            return true;
        }
    }
}

void CloseSDL(SDL_Window*& window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Uint32 convertColour(glm::vec3 colour) {
    int tt;
    Uint8 r, g, b;

    tt = (int)(colour.r * 255);
    if (tt <= 255) r = tt; else r = 255;
    tt = (int)(colour.g * 255);
    if (tt <= 255) g = tt; else g = 255;
    tt = (int)(colour.b * 255);
    if (tt <= 255) b = tt; else b = 255;

    Uint32 rgb;

    //check which order SDL is storing bytes
    rgb = (r << 16) + (g << 8) + b;

    return rgb;
}

void PutPixel32_nolock(SDL_Surface*& surface, int x, int y, Uint32 colour) {
    Uint8* pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = colour;
}

int main()
{
    //SECTON - SDL Setup
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    if (!InitSDL(window, screenSurface)) return -1;




    float fullRenderPercentage = (float)WIDTH * (float)HEIGHT;
    std::vector<GameObject> objects;

    // SETUP COUNTER


    //objects.push_back(GameObject("OBJ files/teapot_simple_smooth.obj", glm::vec3(0,0,-5), glm::vec3(0.5, 0.5, 0), 100.0f));

    std::vector<Shape*> ShapeList;
    ShapeList.push_back(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1.0, 0.32, 0.36), 128)); // red
    ShapeList.push_back(new Sphere(glm::vec3(5, -1, -15), 2,glm::vec3(0.9, 0.76, 0.46), 128)); //green
    ShapeList.push_back(new Sphere(glm::vec3(5, 0, -25), 3,glm::vec3(0.65, 0.77, 0.97), 128)); // blue
    ShapeList.push_back(new Sphere(glm::vec3(-5.5, 0, -15), 3,glm::vec3(0.9, 0.9, 0.9), 128)); //cyan
    ShapeList.push_back(new Plane(glm::vec3(0, -4, 0),glm::vec3(0,1,0), glm::vec3(0.8, 0.8, 0.8),0.0f));
    //ShapeList.push_back(new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::normalize(glm::vec3(0.0,0.6,1.0)),glm::vec3(-0.4,-0.4,1.0),glm::vec3(0.4,-0.4,1.0),glm::vec3(0.7,0.7,0.0),100));

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].AddMesh(ShapeList);
    }

    //PointLight L = PointLight(glm::vec3(1,3,1), glm::vec3(1.0, 1.0, 1.0));
    AreaLight AL = AreaLight(glm::vec3(1,10, 1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(5,5,5), glm::vec3(1,1,1));

    Uint32 runningTimer = SDL_GetTicks();

    glm::vec3** image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];
    glm::vec3 rayOrigin = glm::vec3(0,0,0);
    float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
    glm::vec2 pixelN, pixelR, pixelC;
    
    float tanValue = glm::tan(glm::radians(45.0f) / 2.0f);
    int counter = 0;
    for (int y = 0; y < HEIGHT; y++) {
        //std::cout << (float)counter * 100.0f / fullRenderPercentage << std::endl;
        for (int x = 0; x < WIDTH; x++) {
            
            pixelN = glm::vec2((x + 0.5) / WIDTH, (y + 0.5) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            pixelR = glm::vec2((2 * pixelN.x - 1)*IAR, 1 - 2 * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);

            glm::vec3 PixelColour = glm::vec3(1, 1, 1); // default is white

            HitInfo dump;
            HitInfo smallestH;
            float smallestT = 0;
            int closestShape = -1;
            bool first = true;
            glm::vec3 rayDir = glm::normalize(CamSpace - rayOrigin);
            for (int i = 0; i < ShapeList.size(); i++){
                if (ShapeList[i]->IntersectTest(rayOrigin, rayDir, dump)) { //if true and output is t
                    if (dump.distance < smallestT || first) {
                        smallestT = dump.distance;
                        smallestH = dump;
                        first = false;
                        closestShape = i;//classify it as the smallest
                    }     
                }
            }

            if (closestShape != -1) {
                int HitsDetected = 0;
                for (int i = 0; i < AL.GridPositions.size(); i++)
                {
                    glm::vec3 lightRay = AL.GridPositions[i] - smallestH.intersectionPoint;
                    for (int a = 0; a < ShapeList.size(); a++) {
                        if (closestShape != a) {
                            if (ShapeList[a]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * 1e-06f, glm::normalize(lightRay), dump)) {
                                HitsDetected++;
                                break;
                            }
                        }
                    }
                }



                float success = (float)HitsDetected / (float)AL.GridPositions.size();
                PixelColour = ShapeList[closestShape]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
                PixelColour += ShapeList[closestShape]->GetDiffuseLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal);
                PixelColour += ShapeList[closestShape]->GetSpecularLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal, rayDir);
                PixelColour *= 1.0f - success;


                //if (HitsDetected == 0) {

                //}
                //else {
                //    PixelColour = ShapeList[closestShape]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
                //}
            }

            //if (closestShape != -1) {
            //    glm::vec3 lightRay = L.position - smallestH.intersectionPoint;
            //    bool pass = false;

            //    for (int a = 0; a < ShapeList.size(); a++) {
            //        if (closestShape != a) {
            //            if (ShapeList[a]->IntersectTest(smallestH.intersectionPoint, glm::normalize(lightRay), dump)) {
            //                PixelColour = ShapeList[a]->GetAmbientLight();
            //                pass = true;
            //                break;
            //            }
            //        }

            //    }
            //    if (!pass) {
            //        PixelColour = ShapeList[closestShape]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
            //        PixelColour += ShapeList[closestShape]->GetDiffuseLight(L, glm::normalize(lightRay), smallestH.normal);
            //        PixelColour += ShapeList[closestShape]->GetSpecularLight(L, glm::normalize(lightRay), smallestH.normal, rayDir);
            //    }
            //} 
             //if there is no intersection, it is presumed that there is no collision at all. this means that the final colour is the background colour.
            
            image[x][y] = PixelColour;
            PutPixel32_nolock(screenSurface, x, y, convertColour(image[x][y]));
            counter++;
        }
    }

    std::cout << " time passed: " << (float)(SDL_GetTicks() - runningTimer) / 1000.0f << " seconds" << std::endl;

    SDL_UpdateWindowSurface(window);
    bool quit = false;
    while (!quit) {
        //keyboard Input
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    CloseSDL(window);
    // ray direction = cameraSpace - ray origin (camera origin)

    //std::ofstream ofs("./soft shadows.ppm", std::ios::out | std::ios::binary);
    //ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    //for (unsigned y = 0; y < HEIGHT; ++y) {
    //    for (unsigned x = 0; x < WIDTH; ++x) {
    //        ofs << (unsigned char)(glm::min((float)1, (float)image[x][y].x) * 255) <<
    //            (unsigned char)(glm::min((float)1, (float)image[x][y].y) * 255) <<
    //            (unsigned char)(glm::min((float)1, (float)image[x][y].z) * 255);
    //    }
    //}
    //ofs.close();

    return 0;
}

