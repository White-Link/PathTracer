/**
 * \file object.cpp
 * \brief Implements classes of object.hpp.
 */

#include "object.hpp"


Intersection Sphere::Intersect(const Ray &r) const {
	// Equivalent to find the roots of degree 2 polynomial
	const Point &origin = r.Origin();
	double dot_prod = r.Direction()|(origin-center_);
	double delta = 4*(dot_prod*dot_prod
		- (center_-origin).NormSquared() + radius_*radius_);
	if (delta < 0) {
		return Intersection();
	} else {
		Intersection i1 = Intersection((-2*dot_prod + sqrt(delta))/2, false);
		Intersection i2 = Intersection((-2*dot_prod - sqrt(delta))/2, true);
		// The closest positive intersection is chosen
		return i1 | i2;
	}
}


Vector Sphere::Normal(const Point &p) const {
	Vector direction = p - center_;
	double distance_to_center_squared = direction.NormSquared();
	// Gives an "in" normal (directed towards the center) if v is in the sphere,
	// an "out" normal otherwise.
	direction.Normalize();
	if (distance_to_center_squared < radius_*radius_) {
		return -direction;
	} else {
		return direction;
	}
}


Intersection Plane::Intersect(const Ray &r) const {
	const Vector &direction = r.Direction();
	double dot_prod = (direction | normal_);
	if (dot_prod == 0) {
		// The ray and the plane are parallel
		return Intersection();
	} else {
		return Intersection(-((r.Origin()-point_) | normal_) / dot_prod,
			(normal_|r.Direction()) < 0);
	}
}


Vector Plane::Normal(const Point &p) const {
	Vector normal = normal_;
	normal.Normalize();
	if (((p - point_) | normal) < 0) {
		return -normal;
	} else {
		return normal;
	}
	return normal;
}
