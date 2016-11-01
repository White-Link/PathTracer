/**
 * \file object.cpp
 * \brief Implements classes of object.hpp.
 */

#include "object.hpp"


Intersection Sphere::Intersect(const Ray &r) const {
	// Equivalent to find the roots of degree 2 polynomial
	const Vector &origin = r.Origin();
	double dot_prod = r.Direction()|(origin-center_);
	double delta = 4*(dot_prod-(center_-origin).NormSquared()+radius_*radius_);
	if (delta < 0) {
		return Intersection();
	} else {
		Intersection i1 = Intersection((dot_prod + sqrt(delta))/2);
		Intersection i2 = Intersection((dot_prod - sqrt(delta))/2);
		// The closest positive intersection is chosen
		return i1 | i2;
	}
}
