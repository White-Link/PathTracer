/**
 * \file object.hpp
 * \brief Defines the objects classes that can be used in a scene (Sphere) and
 *        their abstract class Object.
 */

#pragma once

#include <memory>
#include "utils.hpp"


/**
 * \class RawObject
 * \brief Abstract class defining the requirements of any class of object to be
 *        rendered in a scene.
 */
class RawObject {
public:
	/// Computes the Intersection point between the object and the input Ray.
	virtual Intersection Intersect(const Ray &r) const = 0;

	/// Computes the normal unitary vector to the object at the given point.
	virtual Vector Normal(const Vector &p) const = 0;
};


/**
 * \class Sphere
 * \brief Sphere object, defined by a center and a radius.
 */
class Sphere : public RawObject {
private:
	const double radius_; //!< Radius of the Sphere.
	const Vector center_; //!< Center point of the Sphere.

public:
	/// Creates a sphere with given radius and center.
	Sphere(double radius, const Vector &center) :
		radius_{radius}, center_{center}
	{
	}

	Intersection Intersect(const Ray &r) const;

	Vector Normal(const Vector &p) const;
};


/**
 * \class Object
 * \brief Class representing the container of a RawObject.
 *
 * Object contains an unknown RawObject and acts as an interface for usual
 * methods that can be called on objects, like Intersect.
 */
class Object {
private:
	/// The RowObject is stored using a pointer to forget its actual type.
	std::shared_ptr<RawObject> raw_object_;

public:
	/// Creates an empty / invisible object.
	Object() {
		raw_object_.reset(new Sphere(Sphere(-1, Vector(0, 0, 0))));
	}

	/// Creates an object from a Sphere.
	Object(const Sphere &s) {
		raw_object_.reset(new Sphere(s));
	}

	/// Intersection primitive of the contained object.
	Intersection Intersect(const Ray &r) const {
		return raw_object_->Intersect(r);
	}

	/// Computes the normal unitary vector to the object at the given point.
	Vector Normal(const Vector &p) const {
		return raw_object_->Normal(p);
	}
};
