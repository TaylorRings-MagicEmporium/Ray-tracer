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
#include <random>

#include <SDL2/SDL.h>
#undef main

#define PI 3.14159265
const int WIDTH = 640;
const int HEIGHT = 480;

SDL_Event event;

// used for user interface
glm::vec3 CameraOrigin = glm::vec3(0,3,0);
bool Enable_Shadows = false;
int ReflectionRate = 0;
bool EnableReflections = false;
bool WhiteBackground = true;
bool EnableMultiThread = true;
glm::vec3 Background = glm::vec3(1, 1, 1);
int SampleRate = 0;

glm::vec3** image;
std::vector<GameObject*> objects;
std::vector<GameObject> AllObjects;
AreaLight AL = AreaLight(glm::vec3(0, 10, 0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(9, 9, 1), glm::vec3(0,0,0));
float ep = 1e-4;
float IAR = (float)WIDTH / (float)HEIGHT; // these need to be FLOAT
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

int NoRayCasts = WIDTH * HEIGHT;
long int NoRayPrimFuncExecuted = 0; // number of intersections executed (in general)
long int NoRayPrimHits = 0; // number of sucessful hits (when intersection is true)


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

    glm::vec3 PixelColour = Background; // default is white, but can be changed
    glm::vec3 rayDir = glm::normalize(RayDirection);
    for (int a = 0; a < objects.size(); a++) { //for each object
        std::vector<Shape*>returnedShapes;
        if (objects[a]->BB.IntersectTest(rayPos, rayDir,returnedShapes) || objects[a]->AvoidBox) { // get a list of shapes that the ray could hit, or ignore it if the box isn't meant to be used
            for (int b = 0; b < returnedShapes.size(); b++) { // for each shape returned from the intersectTest
                NoRayPrimFuncExecuted++;
                if (returnedShapes[b]->IntersectTest(rayPos, rayDir, dump)) { // if the ray did hit the shape
                    NoRayPrimHits++;
                    if (dump.distance < smallestT) { // see if the shape is closer to the camera than what is already assigned.
                        smallestT = dump.distance;
                        smallestH = dump;
                        closestShape = b;//classify it as the smallest
                        closestObject = a;
                    }
                }
            }
        }

    }

    if (closestObject != -1) { // only calculate the Phong, shadow and reflection if the camera can see the shape

        static thread_local std::mt19937 gen;
        //std::default_random_engine gen;
        std::uniform_int_distribution<int> dist(1, 1000);

        float success = 0;
        if (Enable_Shadows) {
            int HitsDetected = 0;
            for (int i = 0; i < AL.GridPositions.size(); i++) // for each light point on a grid
            {
                bool pass = false;
                glm::vec3 lightRay = AL.GridPositions[i] - smallestH.intersectionPoint; // calculate light ray
                if (SampleRate > 1) {
                    lightRay += AL.GetNoise(dist(gen), 1000);
                }

                for (int a = 0; a < objects.size(); a++) { //for each gameobject
                    if (closestObject != a) { // if the shape is not in current closest object, then continue with intersection
                        std::vector<Shape*>returnedShapes;
                        if (objects[a]->BB.IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), returnedShapes) || objects[a]->AvoidBox) {
                            for (int b = 0; b < returnedShapes.size(); b++) { //and for each shape in that gameobject 
                                NoRayPrimFuncExecuted++;
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
                        std::vector<Shape*>returnedShapes;
                        objects[a]->BB.IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), returnedShapes);
                        for (int b = 0; b < objects[a]->BB.ContainedShapes.size(); b++) {
                            if (closestShape != b) { // test whether its also not the closest shape in that object
                                NoRayPrimFuncExecuted++;
                                if (objects[a]->BB.ContainedShapes[b]->IntersectTest(smallestH.intersectionPoint + smallestH.normal * ep, glm::normalize(lightRay), dump)) {
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

        //the brightness of the pixel depends on how many light rays didn't intersect with anything
        PixelColour = smallestH.shape->GetAmbientLight();
        PixelColour += smallestH.shape->GetDiffuseLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal);
        if (smallestH.shape->Shininess > 0.0f) { // if there is no shininess, then the returned colour should just be the ambient and diffuse
            PixelColour += smallestH.shape->GetSpecularLight(&AL, glm::normalize(AL.position - smallestH.intersectionPoint), smallestH.normal, rayDir);

        }
        PixelColour *= 1.0f - success;

        // if enabled and the shape's shininess is above 0, then reflect (recurse trace) to find the overall colour combined.
        if (smallestH.shape->Shininess > 0.0f) {
            if (CURRENT_DEPTH != MAX_DEPTH) {
                glm::vec3 reflectRay = rayDir - 2.0f * glm::dot(rayDir, smallestH.normal) * smallestH.normal;
                glm::vec3 reflectiveColour = Trace(smallestH.intersectionPoint + smallestH.normal * ep, reflectRay, CURRENT_DEPTH + 1, MAX_DEPTH);

                return glm::lerp(PixelColour, reflectiveColour, smallestH.shape->Shininess/128.0f);
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
            //calculates the pixel position on the screen using the pixel size and get it to between -1 to 1
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
        const int threadCount = 16;

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
    std::cout << "Number of times ray-prim intersect executed: " << NoRayPrimFuncExecuted << std::endl;
    std::cout << "Number of successful ray-Prim hits: " << NoRayPrimHits << std::endl;
    std::cout << std::endl;
    std::cout << "Reflection Level: " << ReflectionRate << std::endl;
    std::cout << "Shadow Level: " << SampleRate << std::endl;
    std::cout << "Multithreading: " << EnableMultiThread << std::endl;
    std::cout << std::endl;

    NoRayPrimHits = 0;
    NoRayPrimFuncExecuted = 0;
}

void ChangeSceen(int SceneNo) {
    if (SceneNo == 1) {
        std::cout << "SHOWING SCENE 1: Just spheres" << std::endl;
        objects.clear();
        objects.push_back(&AllObjects[0]);
        objects.push_back(&AllObjects[1]);
        objects.push_back(&AllObjects[2]);
        objects.push_back(&AllObjects[3]);
        objects.push_back(&AllObjects[4]);
    }
    else if (SceneNo == 2) {
        std::cout << "SHOWING SCENE 2: Spheres and Teapot" << std::endl;
        objects.clear();
        objects.push_back(&AllObjects[0]);
        objects.push_back(&AllObjects[1]);
        objects.push_back(&AllObjects[2]);
        objects.push_back(&AllObjects[3]);
        objects.push_back(&AllObjects[4]);
        objects.push_back(&AllObjects[5]);
    }
    else
    {
        std::cout << "SHOWING SCENE 3: Spheres, Teapot and Complex car" << std::endl;
        objects.clear();
        objects.push_back(&AllObjects[0]);
        objects.push_back(&AllObjects[6]);
        objects.push_back(&AllObjects[7]);
        objects.push_back(&AllObjects[8]);
        objects.push_back(&AllObjects[9]);
        objects.push_back(&AllObjects[10]);
    }
}

void ResetScene() {
    CameraOrigin = glm::vec3(0, 3, 0);
    Enable_Shadows = false;
    ReflectionRate = 0;
    EnableReflections = false;
    WhiteBackground = true;
    EnableMultiThread = true;
    SampleRate = 0;
}

int main()
{
    //SECTON - SDL Setup

    bool first = true;

    if (!InitSDL(window, screenSurface)) return -1;

    //std::vector<GameObject> objects;

    // SETUP COUNTER
    
    GameObject g = GameObject(glm::vec3(0, 0, 0));

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Plane(glm::vec3(-10, 0, -10), glm::vec3(0, 1, 0), glm::vec3(0.8, 0.8, 0.8), 20.0f)); // light gray plane
    g.AvoidBox = true;
    AllObjects.push_back(g);

    //SCENE 1
    g.AddShape(new Sphere(glm::vec3(0, 4, -20), 4, glm::vec3(1.00, 0.32, 0.36), 30.0f)); // red sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(5, 2, -15), 2, glm::vec3(0.9, 0.76, 0.46), 80.0f)); // green sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(5, 3, -25), 3, glm::vec3(0.65, 0.77, 0.97), 80.0f)); // blue sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(-5.5, 3, -15), 3, glm::vec3(0.9, 0.9, 0.9), 5.0f)); // cyan sphere
    AllObjects.push_back(g);

    //SCENE 2 UP
    g = GameObject(glm::vec3(2.5, 2, -7));
    g.AddMesh("OBJ files/teapot_simple_smooth.obj", glm::vec3(0.5, 0.5, 0), 100.0f);
    AllObjects.push_back(g);


    //SCENE 3

    g.AddShape(new Sphere(glm::vec3(0, 1, -5), 1, glm::vec3(1.00,0.32,0.36), 80.0f)); // red sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(8, 2, -15), 2, glm::vec3(0.9, 0.76, 0.46), 80.0f)); // green sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(8, 3, -25), 3, glm::vec3(0.65, 0.77, 0.97), 80.0f)); // blue sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 0, 0));
    g.AddShape(new Sphere(glm::vec3(-10, 3, -10), 3, glm::vec3(0.9, 0.9, 0.9), 80.0f)); // cyan sphere
    AllObjects.push_back(g);

    g = GameObject(glm::vec3(0, 2, -14));
    g.AddMesh("OBJ files/Full_Car_Blend.obj", glm::vec3(0.5, 0.5, 0), 100.0f);
    AllObjects.push_back(g);


    ChangeSceen(1);

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
                    std::cout << "SCENE RESET" << std::endl;
                    ResetScene();
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

                case SDLK_1:
                    ChangeSceen(1);
                    ResetScene();
                    refresh = true;
                    break;

                case SDLK_2:
                    ChangeSceen(2);
                    ResetScene();
                    refresh = true;
                    break;

                case SDLK_3:
                    ChangeSceen(3);
                    ResetScene();
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

            if (first) {
                std::cout << "__________KEY BINDINGS__________" << std::endl;
                std::cout << "w,a,s,d for up,down,left,right" << std::endl;
                std::cout << "- , + for moving backward, forwards" << std::endl;
                std::cout << "# to reset the scene" << std::endl;
                std::cout << " [ , ] for decreasing, increasing the reflection rate" << std::endl;
                std::cout << "b for changing from white to black and vice versa" << std::endl;
                std::cout << "COMMA and PERIOD to decrease, increase the sampling rate" << std::endl;
                std::cout << "m to enable/disable multithreading" << std::endl;
                std::cout << "switch between 1,2 or 3 to change scenes" << std::endl;
                std::cout << std::endl;
                first = false;
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

