/**
 * \file object.cpp
 * \brief Implements classes of object.hpp.
 */

#include "object.hpp"


Sphere::Sphere(double radius, const Vector &center) :
	radius_{radius}, center_{center}
{
}

Intersection Sphere::Intersect(const Ray &r) const {
	// TODO
	return 0;
}
