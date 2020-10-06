#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include "Light.h"
#include "HitInfo.h"


class Shape
{
public:
	glm::vec3 position;
	//glm::vec3 SurColour;
	
	glm::vec3 ambientColour;
	glm::vec3 ambientIntensity = glm::vec3(0.1,0.1,0.1);
	glm::vec3 diffuseColour;
	glm::vec3 SpecularColour = glm::vec3(0.7, 0.7, 0.7);
	float Shininess = 0;
	float ep = 1e-6;

	glm::vec3 Smallest;
	glm::vec3 Biggest;
	


	virtual bool IntersectTest(glm::vec3 RayOrigin, glm::vec3 RayDir, HitInfo& out) = 0;

	glm::vec3 GetAmbientLight() {
		return ambientColour * ambientIntensity;
	}

	glm::vec3 GetDiffuseLight(Light* light, glm::vec3 LI, glm::vec3 IntersectionNormal) {
		return diffuseColour * light->intensity * glm::max(0.0f, glm::dot(LI, IntersectionNormal));
	}

	glm::vec3 GetSpecularLight(Light* light, glm::vec3 LI, glm::vec3 IntersectionNormal, glm::vec3 RayDir) {
		glm::vec3 r = LI - 2.0f * glm::dot(LI, IntersectionNormal) * IntersectionNormal;
		//glm::vec3 r = glm::reflect(LI, IntersectionNormal);
		glm::vec3 g = glm::normalize(r);
		return SpecularColour * light->intensity * glm::pow(glm::max(0.0f, glm::dot(glm::normalize(r), RayDir)), Shininess);
	}
};

