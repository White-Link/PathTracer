/**
 * \file object.hpp
 * \brief Defines the objects classes that can be used in a scene (Sphere) and
 *        their abstract class Object.
 */

#pragma once

#include "utils.hpp"


/**
 * \class Object
 * \brief Abstract class defining the requirements of any class of object to be
 *        rendered in a scene.
 */
class Object {
public:
	/// Computes the Intersection point between the object and the input Ray.
	virtual Intersection Intersect(const Ray &r) const = 0;
};


/**
 * \class Sphere
 * \brief Sphere object, defined by a center and a radius.
 */
class Sphere : public Object {
private:
	double radius_; //!< Radius of the Sphere.
	Vector center_; //!< Center point of the Sphere.

public:
	/// Creates a sphere with given radius and center.
	Sphere(double radius, const Vector &center);

	Intersection Intersect(const Ray &r) const;
};
