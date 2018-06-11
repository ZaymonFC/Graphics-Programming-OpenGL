#include "Plane.h"
#include <glm/detail/func_geometric.inl>

Plane::Plane( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3) {
	set3Points(v1,v2,v3);
}

void Plane::set3Points( glm::vec3 &v1,  glm::vec3 &v2,  glm::vec3 &v3) {
	auto aux1 = v1 - v2;
	auto aux2 = v3 - v2;

	normal = aux2 * aux1;

	normal = glm::normalize(normal);
	point = glm::vec3(v2);
	d = -(glm::dot(normal, point));
}

void Plane::setNormalAndPoint(glm::vec3 &normal, glm::vec3 &point) {

	this->normal = glm::vec3(normal);
	this->normal = glm::normalize(this->normal);
	d = -(glm::dot(this->normal, point));
}

void Plane::setCoefficients(float a, float b, float c, float d) {

	// set the normal vector
	normal = glm::vec3(a,b,c);
	//compute the lenght of the vector
	auto l = normal.length();
	// normalize the vector
	normal = glm::vec3(a/l,b/l,c/l);
	// and divide d by th length as well
	this->d = d/l;
}

float Plane::distance(glm::vec3 &p) {
	return d + glm::dot(normal, p);
}
