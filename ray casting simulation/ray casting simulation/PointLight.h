#pragma once
#include <glm/glm.hpp>
#include "Light.h"
class PointLight : public Light
{
public:

	PointLight(glm::vec3 position, glm::vec3 intensity);

};

