#pragma once
#include <glm/glm.hpp>
#include "Light.h"
class PointLight : public Light
{
public:
	// A specific light that has a point that light comes out of
	PointLight(glm::vec3 position, glm::vec3 intensity);

};

