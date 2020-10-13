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

glm::vec3 CameraOrigin = glm::vec3(0);
glm::vec3** image;
std::vector<GameObject> objects;
AreaLight AL = AreaLight(glm::vec3(1, 10, 1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(5, 5, 5), glm::vec3(1, 1, 1));
float ep = 1e-6;
float MAX_SHININESS = 128;


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

glm::vec3 Trace(glm::vec3 rayPos, glm::vec3 RayDirection, int CURRENT_DEPTH, int MAX_DEPTH) {
    HitInfo dump;
    HitInfo smallestH;
    float smallestT = 1e06;
    int closestShape = -1;
    int closestObject = -1;

    glm::vec3 PixelColour = glm::vec3(1, 1, 1); // default is white
    glm::vec3 rayDir = glm::normalize(RayDirection - rayPos);
    for (int a = 0; a < objects.size(); a++) {
        if (objects[a].BB.IntersectTest(rayPos, rayDir) || objects[a].AvoidBox) {
            for (int b = 0; b < objects[a].ShapeList.size(); b++) {
                //NoRayPrimIntersectFunctions++;
                if (objects[a].ShapeList[b]->IntersectTest(rayPos, rayDir, dump)) {//if true and output is t
                    //NoRayPrimHits++;
                    if (dump.distance < smallestT) {
                        smallestT = dump.distance;
                        smallestH = dump;
                        closestShape = b;//classify it as the smallest
                        closestObject = a;
                    }
                }
            }
        }

    }

    if (closestObject != -1) {


        int HitsDetected = 0;
        for (int i = 0; i < AL.GridPositions.size(); i++) // for each light point on a grid
        {
            bool pass = false;
            glm::vec3 lightRay = AL.GridPositions[i] - smallestH.intersectionPoint; // calculate light ray
            for (int a = 0; a < objects.size(); a++) { //for each gameobject
                if (closestObject != a) { // if the shape is not in current closest object, then continue with intersection
                    if (objects[a].BB.IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay)) || objects[a].AvoidBox) {
                        for (int b = 0; b < objects[a].ShapeList.size(); b++) { //and for each shape in that gameobject 
                            //NoRayPrimIntersectFunctions++;
                            if (objects[a].ShapeList[b]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), dump)) {
                                //NoRayPrimHits++;
                                HitsDetected++;
                                pass = true;
                                break;
                            }
                        }
                    }

                }
                else {
                    for (int b = 0; b < objects[a].ShapeList.size(); b++) {
                        if (closestShape != b) { // test whether its also not the closest shape in that object
                            //NoRayPrimIntersectFunctions++;
                            if (objects[a].ShapeList[b]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), dump)) {
                                //NoRayPrimHits++;
                                HitsDetected++;
                                pass = true;
                                break;
                            }
                        }
                    }
                }

                if (pass) {
                    break;
                }
            }
        }

        float success = (float)HitsDetected / (float)AL.GridPositions.size();
        PixelColour = objects[closestObject].ShapeList[closestShape]->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
        PixelColour += objects[closestObject].ShapeList[closestShape]->GetDiffuseLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal);
        PixelColour += objects[closestObject].ShapeList[closestShape]->GetSpecularLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal, rayDir);
        PixelColour *= 1.0f - success;

        if (objects[closestObject].ShapeList[closestShape]->Shininess > 0.0f) {
            if (CURRENT_DEPTH + 1 != MAX_DEPTH+1) {
                //glm::vec3 reflectRay = glm::reflect(rayDir, smallestH.normal);
                glm::vec3 reflectRay = rayDir - 2.0f * glm::dot(rayDir, smallestH.normal) * smallestH.normal;
                //float ShiniRate = objects[closestObject].ShapeList[closestShape]->Shininess / MAX_SHININESS;
                return (PixelColour +  Trace(smallestH.intersectionPoint + smallestH.normal * ep, reflectRay, CURRENT_DEPTH + 1, MAX_DEPTH))/3.0f  ;
            }
            else {
                return PixelColour;
            }
        }
    }
    else {
        return PixelColour;
    }

    // AFTER RUN
    return PixelColour;

}

void RefreshScreen(SDL_Surface* screenSurface) {
    Uint32 runningTimer = SDL_GetTicks();
    int NoRayCasts = WIDTH * HEIGHT;
    int NoRayPrimIntersectFunctions = 0;
    int NoRayPrimHits = 0;

    for (int i = 0; i < objects.size(); i++) {
        objects[i].BB.UpdatePlaneLimits();
    }
    //glm::vec3 rayOrigin = glm::vec3(0, 0, 0);
    float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
    glm::vec2 pixelN, pixelR, pixelC;

    float tanValue = glm::tan(glm::radians(60.0f) / 2.0f);
    int counter = 0;
    for (int y = 0; y < HEIGHT; y++) {
        //std::cout << (float)counter * 100.0f / fullRenderPercentage << std::endl;
        for (int x = 0; x < WIDTH; x++) {

            pixelN = glm::vec2((x + 0.5f) / WIDTH, (y + 0.5f) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            pixelR = glm::vec2((2.0f * pixelN.x - 1.0f) * IAR, 1.0f - 2.0f * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);



            glm::vec3 PixelColour = Trace(CameraOrigin, CamSpace, 0, 2);

            image[x][y] = PixelColour;
            PutPixel32_nolock(screenSurface, x, y, convertColour(image[x][y]));
            counter++;
        }
    }

    std::cout << "__________ STATISTICS __________" << std::endl;
    std::cout << "Time passed: " << (float)(SDL_GetTicks() - runningTimer) / 1000.0f << " seconds" << std::endl;
    std::cout << "Total number of rays in object detection: " << NoRayCasts << std::endl;
    std::cout << "Number of times ray-prim intersect is called: " << NoRayPrimIntersectFunctions << std::endl;
    std::cout << "Number of successful ray-Prim hits: " << NoRayPrimHits << std::endl;
}

int main()
{
    //SECTON - SDL Setup
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    if (!InitSDL(window, screenSurface)) return -1;

    //std::vector<GameObject> objects;

    // SETUP COUNTER
    
    GameObject g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(0, 0, -20), 4, glm::vec3(1.0, 0.32, 0.36), 128)); // red sphere
    objects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(5, -1, -15), 2, glm::vec3(0.9, 0.76, 0.46), 128)); // green sphere
    objects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(5, 0, -25), 3, glm::vec3(0.65, 0.77, 0.97), 128)); // blue sphere
    objects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(0.9, 0.9, 0.9), 128)); // cyan sphere
    objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Sphere(glm::vec3(-3, 0, -5), 3, glm::vec3(0.5, 0.3, 0.2), 128));
    //objects.push_back(g);

    //g = GameObject(glm::vec3(2, 0, -10));
    //g.AddMesh("OBJ files/teapot_simple_smooth.obj", glm::vec3(0.5, 0.5, 0), 100.0f);
    //objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::normalize(glm::vec3(0.0, 0.6, 1.0)), glm::vec3(-0.4, -0.4, 1.0), glm::vec3(0.4, -0.4, 1.0), glm::vec3(0.7, 0.7, 0.0), 100));
    //objects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Plane(glm::vec3(0, -4, 0), glm::vec3(0, 1, 0), glm::vec3(0.8, 0.8, 0.8), 0.0f)); // light gray plane
    g.AvoidBox = true;
    objects.push_back(g);
    
    //ShapeList.push_back(new Triangle(glm::vec3(0, 1, -2), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::normalize(glm::vec3(0.0,0.6,1.0)),glm::vec3(-0.4,-0.4,1.0),glm::vec3(0.4,-0.4,1.0),glm::vec3(0.7,0.7,0.0),100));



    //STATS 
    //float fullRenderPercentage = (float)WIDTH * (float)HEIGHT;

    image = new glm::vec3 * [WIDTH];
    for (int i = 0; i < WIDTH; i++) image[i] = new glm::vec3[HEIGHT];

    bool quit = false;
    bool refresh = true;
    while (!quit) {
        //keyboard Input
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {

                switch (event.key.keysym.sym) {
                case SDLK_w:
                    CameraOrigin += glm::vec3(0, -0.1, 0);
                    refresh = true;
                    break;

                case SDLK_a:
                    CameraOrigin += glm::vec3(0.1, 0, 0);
                    refresh = true;
                    break;

                case SDLK_s:
                    CameraOrigin += glm::vec3(0, 0.1, 0);
                    refresh = true;
                    break;

                case SDLK_d:
                    CameraOrigin += glm::vec3(-0.1, 0, 0);
                    refresh = true;
                    break;

                case SDLK_MINUS:
                    CameraOrigin += glm::vec3(0, 0, -0.1);
                    refresh = true;
                    break;

                case SDLK_EQUALS:
                    CameraOrigin += glm::vec3(0, 0, 0.1);
                    refresh = true;
                    break;
                }
            }
        }

        if (refresh) {
            std::cout << "REFRESHING" << std::endl;
            RefreshScreen(screenSurface);
            SDL_UpdateWindowSurface(window);
            refresh = false;
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

