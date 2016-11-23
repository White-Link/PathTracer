/**
 * \file object.hpp
 * \brief Defines the objects classes that can be used in a scene (Sphere,
 *        Plane) and their abstract class Object.
 */

#pragma once

#include <memory>
#include <random>
#include <chrono>
#include "utils.hpp"
#include "material.hpp"


class AABB;


/**
 * \class RawObject
 * \brief Abstract class defining the requirements of any class of object to be
 *        rendered in a scene.
 */
class RawObject {
protected:
	Material material_; //!< Material of the object.

	/// Indicates if the object has null volume (if is not empty).
	bool is_flat_;

public:
	/// Default constructor of RawObject.
	RawObject(const Material& material, bool is_flat) :
		material_{material},
		is_flat_{is_flat}
	{
	}

	/// Computes the Intersection point between the object and the input Ray.
	virtual Intersection Intersect(const Ray &r) const = 0;

	/// Computes the normal unitary vector to the object at the given point.
	virtual Vector Normal(const Point &p) const = 0;

	/// Outputs a bounding box of the object as an AABB.
	virtual AABB BoundingBox() const = 0;

	/// Outputs the Material of the object.
	const Material& ObjectMaterial() const {
		return material_;
	}

	/// Indicates if the object has null volume (if is not empty).
	bool IsFlat() const {
		return is_flat_;
	}
};


/**
 * \class Sphere
 * \brief Sphere object, defined by a center and a radius.
 */
class Sphere : public RawObject {
private:
	const double radius_; //!< Radius of the Sphere.
	const Point center_; //!< Center point of the Sphere.

public:
	/// Creates a sphere with given radius and center.
	Sphere(double radius, const Point &center,
		const Material &material=Material()
	) :
		RawObject{material, false},
		radius_{radius},
		center_{center}
	{
	}

	Intersection Intersect(const Ray &r) const;

	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;
};


/**
 * \class Plane
 * \brief Plane object, defined by a point and a normal.
 */
class Plane : public RawObject {
private:
	const Point point_;   //!< Point of the Plane.
	const Vector normal_; //!< Normal of the Plane.

public:
	/// Creates a Plane with given normal and a point.
	Plane(const Point &point, const Vector &normal,
		const Material &material=Material()
	) :
		RawObject{material, true},
		point_{point},
		normal_{normal}
	{
	}

	Intersection Intersect(const Ray &r) const;

	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;
};


/**
 * \class Triangle
 * \brief Triangle object, defines by three points.
 *
 * \remark The normals defined at each vertex of the Triangle should point to
 *         the same half-space.
 */
class Triangle : public RawObject {
private:
	const Point p1_; //!< First point defining the Triangle.
	const Point p2_; //!< Second point defining the Triangle.
	const Point p3_; //!< Third point defining the Triangle.

	/// Geometric normal of the triangle (normal of the embedding plane). Has
	/// the same direction as normal_.
	Vector normal_plane_;

	const Vector normal1_; //!< Normal of the triangle of the first vertex.
	const Vector normal2_; //!< Normal of the triangle of the second vertex.
	const Vector normal3_; //!< Normal of the triangle of the third vertex.

	/**
	 * \fn Vector BarycenticCoordinates(const Point &p) const
	 * \brief Computes the barycentric coordinates of the input point.
	 * \input p Point assumed to lie in the embedding plane of the triangle.
	 * \return A Vector \f$\lambda_1, \lambda_2, \lambda_3\f$ of the barycentric
	 *         coordinates of, respectively, the first, second and third vertex.
	 */
	 Vector BarycenticCoordinates(const Point &p) const;

public:
	/// Creates a Triangle from its three vertices, assumed to be pairwise
	/// distinct, using the normal of their embedding plane.
	Triangle(const Point &p1, const Point &p2, const Point &p3,
		const Material &material=Material()
	) :
		RawObject{material, true},
		p1_{p1},
		p2_{p2},
		p3_{p3},
		normal_plane_{(p2-p1)^(p3-p1)},
		normal1_{normal_plane_},
		normal2_{normal_plane_},
		normal3_{normal_plane_}
	{
		normal_plane_.Normalize();
	}

	/// Creates a Triangle from its three vertices, assumed to be pairwise
	/// distinct, using a given normal per vertex (assumed to be normalized)
	Triangle(const Point &p1, const Point &p2, const Point &p3,
		const Vector &normal1, const Vector &normal2, const Vector &normal3,
		const Material &material=Material()
	) :
		RawObject{material, true},
		p1_{p1},
		p2_{p2},
		p3_{p3},
		normal_plane_{(p2-p1)^(p3-p1)},
		normal1_{normal1},
		normal2_{normal2},
		normal3_{normal3}
	{
		normal_plane_.Normalize();
	}

	Intersection Intersect(const Ray &r) const;

	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;
};


/**
 * \class AABB
 * \brief Box object, defined by two extremal points.
 * \warning Should not be used in an actual scene; designed for BVH heuristics.
 */
class AABB : public RawObject {
private:
	Point p1_; //!< First extremal point (with minimum coordinates).
	Point p2_; //!< Second extremal point (with maximum coordinates).

public:
	/// Creates an empty box.
	AABB() :
	 	RawObject{Material{}, false}
	{
	}

	/// Creates a Box from two extremal points.
	AABB(const Point &p1, const Point &p2,
		const Material &material=Material()
	) :
		RawObject{material, false},
		p1_{p1},
		p2_{p2}
	{
	}

	/// Returns the minimum and maximum x of the box.
	std::pair<double, double> XMinMax() const;

	/// Returns the minimum and maximum y of the box.
	std::pair<double, double> YMinMax() const;

	/// Returns the minimum and maximum z of the box.
	std::pair<double, double> ZMinMax() const;

	/// Outputs the centroid of the box.
	Point Centroid() const;

	Intersection Intersect(const Ray &r) const;

	/// \warning Does not return the normal of the object. Should not be used.
	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;

	/// Bounding box of two AABBs.
	AABB operator||(const AABB &aabb) const;
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
	std::shared_ptr<RawObject> raw_object_;

public:
	/// Creates an empty / invisible object.
	Object() :
		raw_object_{new Sphere(Sphere(-1, Point(0, 0, 0), Material{}))}
	{
	}

	/// Creates an object from a Sphere and a Material.
	Object(const Sphere &s) :
		raw_object_{new Sphere{s}}
	{
	}

	/// Creates an object from a Plane and a Material.
	Object(const Plane &plane) :
		raw_object_{new Plane{plane}}
	{
	}

	/// Creates an object from an AABB.
	Object(const AABB &aabb) :
		raw_object_{new AABB{aabb}}
	{
	}

	/// Outputs the Material of the object.
	const Material& ObjectMaterial() const {
		return raw_object_->ObjectMaterial();
	}

	/// Indicates if the object has null volume (if is not empty).
	bool IsFlat() const {
		return raw_object_->IsFlat();
	}

	/// Intersection primitive of the contained object.
	Intersection Intersect(const Ray &r) const {
		return raw_object_->Intersect(r);
	}

	/// Computes the normal unitary vector to the object at the given point.
	Vector Normal(const Vector &p) const {
		return raw_object_->Normal(p);
	}

	/// Outputs a bounding box of the object as an AABB.
	AABB BoundingBox() const {
		return raw_object_->BoundingBox();
	}

};
