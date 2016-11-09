/**
 * \file object.hpp
 * \brief Defines the objects classes that can be used in a scene (Sphere,
 *        Plane) and their abstract class Object.
 */

#pragma once

#include <memory>
#include "utils.hpp"
#include "material.hpp"


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
 * \class Plane
 * \brief Plane object, defined by a point and a normal.
 */
class Plane : public RawObject {
private:
	const Vector point_;  //!< Point of the Plane.
	const Vector normal_; //!< Normal of the Plane.

public:
	/// Creates a Plane with given normal and a point.
	Plane(const Vector &point, const Vector &normal) :
		point_{point}, normal_{normal}
	{
	}

	Intersection Intersect(const Ray &r) const;

	Vector Normal(const Vector &p) const;
};


/**
 * \class Object
 * \brief Class representing the container of a RawObject, and assigning to it a
 *        Material.
 *
 * Object contains an unknown RawObject and acts as an interface for usual
 * methods that can be called on objects, like Intersect.
 */
class Object {
private:
	/// The RawObject is stored using a pointer to forget its actual type.
	const std::shared_ptr<RawObject> raw_object_;

	const Material material_; //!< Material of the object.

	/// Indicates if the object has null volume (if is not empty).
	const bool is_flat_;

public:
	/// Creates an empty / invisible object.
	Object() :
		is_flat_{true}, raw_object_{new Sphere(Sphere(-1, Vector(0, 0, 0)))}
	{
	}

	/// Creates an object from a Sphere and a Material.
	Object(const Sphere &s, const Material &material=Material())
		: material_{material}, raw_object_{new Sphere(s)}, is_flat_{false}
	{
	}

	/// Creates an object from a Plane and a Material.
	Object(const Plane &plane, const Material &material=Material())
		: material_{material}, raw_object_{new Plane(plane)}, is_flat_{true}
	{
	}

	/// Outputs the Material of the object.
	const Material& ObjectMaterial() const {
		return material_;
	}

	/// Indicates if the object has null volume (if is not empty).
	bool IsFlat() const {
		return is_flat_;
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
