#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <glm/glm.hpp>
#include <vector>

// loadOBJ manages how data is loaded into the program
bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals
);



#endif
