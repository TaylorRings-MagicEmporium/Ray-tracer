#include "Sphere.h"

Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 DiffuseColour, float shininess) {
	this->position = position;
	this->radius = radius;

    this->diffuseColour = DiffuseColour;
    this->ambientColour = DiffuseColour;
    this->Shininess = shininess;

    this->Smallest = position - glm::vec3(radius);
    this->Biggest = position + glm::vec3(radius);
    
}

bool Sphere::IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out) {
    float t0, t1; // two maximum possible points of intersection

    glm::vec3 l = position - RayOrigin; // vector length between ray origin and sphere origin
    float Tca = glm::dot(l, RayDir); // the projection of the vector length onto the direction
    if (Tca < 0) return false; // if it's negative, due to the projection being in the opposite direction of the ray direction, false.

    float s = glm::dot(l, l) - Tca * Tca; // squared magnitude of l and Tca
    if (s > radius * radius) return false; //if the squared length between the centre to right angled to the ray is bigger than the squared radius, then it's outside the sphere

    float Thc = glm::sqrt(radius * radius - s); // vector subtraction to find the length of Thc (from p to centre to middle)
    t0 = Tca - Thc;
    t1 = Tca + Thc;

    if (t0 > t1) std::swap(t0, t1); // smaller value is closer to ray origin than bigger value
    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) {
            return false; // if both values are negative then this is a incorrect ray intersection;
        }
    }

    //float t = t0; // the first thing we will render is the closest to the camera (the bigger value cannot be seen if opaque)
    out.distance = t0;
    out.intersectionPoint = RayOrigin + t0 * RayDir;
    out.normal = glm::normalize(out.intersectionPoint - position);
    return true;
}