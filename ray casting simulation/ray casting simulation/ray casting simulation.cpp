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
#include "BoundingBox.h"
#include <thread>


#include <SDL2/SDL.h>
#undef main

#define PI 3.14159265
const int WIDTH = 640;
const int HEIGHT = 480;

SDL_Event event;

glm::vec3 CameraOrigin = glm::vec3(0,1,0);
bool Enable_Shadows = false;
int ReflectionRate = 0;
bool EnableReflections = false;
bool WhiteBackground = true;
bool EnableMultiThread = true;
glm::vec3 Background = glm::vec3(1, 1, 1);
int SampleRate = 0;

glm::vec3** image;
std::vector<GameObject> objects;
AreaLight AL = AreaLight(glm::vec3(1, 10, 1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(5, 5, 5), glm::vec3(0,0,0));
float ep = 1e-4;
float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

int NoRayCasts = WIDTH * HEIGHT;
long int NoRayPrimIntersectFunctions = 0;
long int NoBoxRayIntersect = 0;
long int NoRayPrimHits = 0;


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

    glm::vec3 PixelColour = Background; // default is white
    glm::vec3 rayDir = glm::normalize(RayDirection);
    for (int a = 0; a < objects.size(); a++) {
        NoBoxRayIntersect++;
        std::vector<Shape*>returnedShapes;
        if (objects[a].BB.IntersectTest(rayPos, rayDir,returnedShapes) || objects[a].AvoidBox) {
            for (int b = 0; b < returnedShapes.size(); b++) {
                NoRayPrimIntersectFunctions++;
                if (returnedShapes[b]->IntersectTest(rayPos, rayDir, dump)) {//if true and output is t
                    NoRayPrimHits++;
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

        float success = 0;
        if (Enable_Shadows) {
            int HitsDetected = 0;
            for (int i = 0; i < AL.GridPositions.size(); i++) // for each light point on a grid
            {
                bool pass = false;
                glm::vec3 lightRay = AL.GridPositions[i] - smallestH.intersectionPoint; // calculate light ray
                for (int a = 0; a < objects.size(); a++) { //for each gameobject
                    if (closestObject != a) { // if the shape is not in current closest object, then continue with intersection
                        NoBoxRayIntersect++;
                        std::vector<Shape*>returnedShapes;
                        if (objects[a].BB.IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), returnedShapes) || objects[a].AvoidBox) {
                            for (int b = 0; b < returnedShapes.size(); b++) { //and for each shape in that gameobject 
                                NoRayPrimIntersectFunctions++;
                                if (returnedShapes[b]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), dump)) {
                                    NoRayPrimHits++;
                                    HitsDetected++;
                                    pass = true;
                                    break;
                                }
                            }
                        }

                    }
                    else {
                        for (int b = 0; b < objects[a].BB.ContainedShapes.size(); b++) {
                            if (closestShape != b) { // test whether its also not the closest shape in that object
                                NoRayPrimIntersectFunctions++;
                                if (objects[a].BB.ContainedShapes[b]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), dump)) {
                                    NoRayPrimHits++;
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

            success = (float)HitsDetected / (float)AL.GridPositions.size();
        }

        PixelColour = smallestH.shape->GetAmbientLight(); // we know now that at least one object is being intersected, so the colour is from the background to the hit.shape
        PixelColour += smallestH.shape->GetDiffuseLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal);
        if (smallestH.shape->Shininess > 0.0f) {
            PixelColour += smallestH.shape->GetSpecularLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal, rayDir);

        }
        PixelColour *= 1.0f - success;

        if (smallestH.shape->Shininess > 0.0f) {
            if (CURRENT_DEPTH != MAX_DEPTH) {
                //glm::vec3 reflectRay = glm::reflect(rayDir, smallestH.normal);
                glm::vec3 reflectRay = rayDir - 2.0f * glm::dot(rayDir, smallestH.normal) * smallestH.normal;
                glm::vec3 reflectiveColour = Trace(smallestH.intersectionPoint + smallestH.normal * ep, reflectRay, CURRENT_DEPTH + 1, MAX_DEPTH);
                //if (reflectiveColour != Background) {
                //    return glm::lerp(PixelColour, reflectiveColour, 0.7f);
                //}
                //else {
                //    return PixelColour;
                //}
                return glm::lerp(PixelColour, reflectiveColour, smallestH.shape->Shininess/128.0f);
                //return PixelColour + reflectiveColour;
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

void MultiThread(int start, int end) {
    for (int x = start; x < end; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            float tanValue = glm::tan(glm::radians(60.0f) / 2.0f);

            glm::vec2 pixelN = glm::vec2((x + 0.5f) / WIDTH, (y + 0.5f) / HEIGHT); //0.5 to get middle of pixel (normallises the pixel coords with the image size. so, between 0,1);
            glm::vec2 pixelR = glm::vec2((2.0f * pixelN.x - 1.0f) * IAR, 1.0f - 2.0f * pixelN.y); // remap to the current image size, assuming that the y height doesn't change to (-1,1)
            glm::vec2 pixelC = pixelR * tanValue;

            glm::vec3 CamSpace = glm::vec3(pixelC.x, pixelC.y, -1);

            glm::vec3 PixelColour = glm::vec3(0);
            if (EnableReflections) {
                PixelColour = Trace(CameraOrigin, CamSpace, 0, ReflectionRate);
            }
            else {
                PixelColour = Trace(CameraOrigin, CamSpace, 0, 0);
            }


            image[x][y] = PixelColour;
            PutPixel32_nolock(screenSurface, x, y, convertColour(image[x][y]));
        }
    }
}

void RefreshScreen(SDL_Surface* screenSurface) {
    Uint32 runningTimer = SDL_GetTicks();
    
    if (WhiteBackground) {
        Background = glm::vec3(1);
    }
    else {
        Background = glm::vec3(0);
    }

    

    if (EnableMultiThread) {
        std::vector<std::thread> threads;
        const int threadCount = 32;

        for (int i = 0; i < threadCount; i++) {
            threads.push_back(std::thread(MultiThread, i * WIDTH / threadCount, (i + 1) * WIDTH / threadCount));
        }

        for (int i = 0; i < threadCount; i++) {
            threads[i].join();
        }
    }
    else {
        MultiThread(0, WIDTH);
    }





    std::cout << "__________ STATISTICS __________" << std::endl;
    std::cout << "Time passed: " << (float)(SDL_GetTicks() - runningTimer) / 1000.0f << " seconds" << std::endl;
    std::cout << "Total number of rays in object detection: " << NoRayCasts << std::endl;
    std::cout << "Number of times ray-prim intersect is called: " << NoRayPrimIntersectFunctions << std::endl;
    std::cout << "Number of successful ray-Prim hits: " << NoRayPrimHits << std::endl;
    std::cout << "Number of Bounding Box tests: " << NoBoxRayIntersect << std::endl;
    std::cout << std::endl;
    std::cout << "Reflection Level: " << ReflectionRate << std::endl;
    std::cout << "Shadow Level: " << SampleRate << std::endl;
    std::cout << "Multithreading: " << EnableMultiThread << std::endl;

    NoRayPrimHits = 0;
    NoRayPrimIntersectFunctions = 0;
    NoBoxRayIntersect = 0;
}

int main()
{
    //SECTON - SDL Setup

    if (!InitSDL(window, screenSurface)) return -1;

    //std::vector<GameObject> objects;

    // SETUP COUNTER
    
    GameObject g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Sphere(glm::vec3(0, 4, -20), 4, glm::vec3(1.00,0.32,0.36), 80.0f)); // red sphere
    //objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Sphere(glm::vec3(5, 2, -15), 2, glm::vec3(0.9, 0.76, 0.46), 80.0f)); // green sphere
    //objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Sphere(glm::vec3(5, 3, -25), 3, glm::vec3(0.65, 0.77, 0.97), 80.0f)); // blue sphere
    //objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Sphere(glm::vec3(-5.5, 3, -10), 3, glm::vec3(0.9, 0.9, 0.9), 80.0f)); // cyan sphere
    //objects.push_back(g);

    g = GameObject(glm::vec3(2, 2, -7));
    g.AddMesh("OBJ files/teapot_simple_smooth.obj", glm::vec3(0.5, 0.5, 0), 100.0f);
    objects.push_back(g);

    //g = GameObject(glm::vec3(0, 0, 0));
    //g.AddShape(new Triangle(glm::vec3(-1, 1, -1), glm::vec3(-1.9, -1, -2), glm::vec3(1.6, -0.5, -2), glm::normalize(glm::vec3(0.0, 0.6, 1.0)), glm::vec3(-0.4, -0.4, 1.0), glm::vec3(0.4, -0.4, 1.0), glm::vec3(0.7, 0.7, 0.0), 20));
    //objects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Plane(glm::vec3(-10, 0, -10), glm::vec3(0, 1, 0), glm::vec3(0.8, 0.8, 0.8), 20.0f)); // light gray plane
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
                    CameraOrigin += glm::vec3(0, 0.1, 0);
                    refresh = true;
                    break;

                case SDLK_a:
                    CameraOrigin += glm::vec3(-0.1, 0, 0);
                    refresh = true;
                    break;

                case SDLK_s:
                    CameraOrigin += glm::vec3(0, -0.1, 0);
                    refresh = true;
                    break;

                case SDLK_d:
                    CameraOrigin += glm::vec3(0.1, 0, 0);
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

                case SDLK_HASH:
                    std::cout << "CAMERA RESET" << std::endl;
                    CameraOrigin = glm::vec3(0,1,0);
                    refresh = true;
                    break;

                case SDLK_LEFTBRACKET:
                    ReflectionRate -= 1;
                    if (ReflectionRate < 0) {
                        EnableReflections = false;
                        ReflectionRate = 0;
                        std::cout << "REFLECTIONS DISABLED" << std::endl;
                    }
                    refresh = true;
                    break;

                case SDLK_RIGHTBRACKET:
                    ReflectionRate++;
                    if (ReflectionRate > 0) {
                        EnableReflections = true;
                        std::cout << "REFLECTIONS ENABLED" << std::endl;
                    }
                    refresh = true;
                    break;

                case SDLK_b:
                    WhiteBackground = !WhiteBackground;
                    refresh = true;
                    break;
                
                case SDLK_COMMA:
                    SampleRate -= 1;
                    if (SampleRate < 1) {
                        SampleRate = 0;
                        Enable_Shadows = false;
                        std::cout << "SHADOW OFF" << std::endl;
                    }
                    AL.ChangeSampleRate(SampleRate);
                    refresh = true;
                    break;

                case SDLK_PERIOD:
                    SampleRate++;
                    std::cout << SampleRate << std::endl;
                    if (SampleRate > 0) {
                        Enable_Shadows = true;
                        std::cout << "SHADOW ON" << std::endl;
                    }
                    AL.ChangeSampleRate(SampleRate);
                    refresh = true;
                    break;
                
                case SDLK_m:

                    EnableMultiThread = !EnableMultiThread;
                    if (EnableMultiThread) {
                        std::cout << "MULTITHREADING ENABLED" << std::endl;
                    }
                    else {
                        std::cout << "MULTITHREADING DISABLED" << std::endl;
                    }
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

    for (int i = 0; i < objects.size(); i++) {
        for (int b = 0; b < objects[i].BBshapes.size(); b++) {
            //delete objects[b].BBshapes[b];
        }
        objects[i].BBshapes.clear();
    }

    return 0;
}

