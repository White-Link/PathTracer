/**
 * \file object.hpp
 * \brief Defines the main objects classes, their abstract model and an object
 *        wrapper.
 */

#pragma once

#include <memory>
#include <random>
#include <chrono>
#include "CImg.h"
#include "utils.hpp"
#include "material.hpp"


class AABB;
class Mesh;


/**
 * \class RawObject
 * \brief Abstract class defining the requirements of any class of object to be
 *        rendered in a scene.
 */
class RawObject {
protected:
	Material material_; //!< Material of the object.

	/// Indicates if the object has null volume (if it is not empty).
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

	/// Computes the normalized normal vector to the object at the given point.
	virtual Vector Normal(const Point &p) const = 0;

	/// Outputs a bounding box of the object as an AABB.
	virtual AABB BoundingBox() const = 0;

	/**
	 * \fn virtual Vector DiffuseColor(const Point &p) const
	 * \brief Outputs the diffuse color of the object at the input point.
	 *
	 * By default, corresponds to the material's diffuse color.
	 */
	virtual Vector DiffuseColor(const Point &p) const {
		return material_.DiffuseColor();
	}

	/**
	 * \fn virtual Vector SpecularColor(const Point &p) const
	 * \brief Outputs the specular color of the object at the input point.
	 *
	 * By default, corresponds to the material's specular color.
	 */
	virtual Vector SpecularColor(const Point &p) const {
		return material_.SpecularColor();
	}

	/// Outputs the Material of the object.
	inline const Material& ObjectMaterial() const {
		return material_;
	}

	/// Indicates if the object has null volume (if is not empty).
	inline bool IsFlat() const {
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
	const Point center_;  //!< Center point of the Sphere.

public:
	/// Creates a sphere with given radius and center.
	Sphere(
		double radius,
		const Point &center,
		const Material &material=Material{}
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
	const Vector normal_; //!< Normal of the Plane, assumed to be normalized.

public:
	/// Creates a Plane with given normal and a point.
	Plane(
		const Point &point,
		const Vector &normal,
		const Material &material=Material{}
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
 * \brief Triangle object, defines by three points, possibly associated to a
 *        texture.
 */
class Triangle : public RawObject {
private:
	const Point p1_; //!< First point defining the Triangle.
	const Point p2_; //!< Second point defining the Triangle.
	const Point p3_; //!< Third point defining the Triangle.

	/**
	 * \brief Normal of the embedding plane of the Triangle.
	 * \note Oriented towards the same half-space as normal1_.
	 */
	Vector normal_plane_;

	Vector normal1_; //!< Normal of the triangle at the first vertex.
	Vector normal2_; //!< Normal of the triangle at the second vertex.
	Vector normal3_; //!< Normal of the triangle at the third vertex.

	/// Diffuse texture associated to this triangle.
	const std::shared_ptr<cimg_library::CImg<unsigned char>> diffuse_texture_;

	/// Specular texture associated to this triangle.
	const std::shared_ptr<cimg_library::CImg<unsigned char>> specular_texture_;

	const bool has_uv_coordinates_; //!< Shows the validity of UV coordinates.
	const float u1_; //!< First UV coordinate associated to p1_.
	const float v1_; //!< Second UV coordinate associated to p1_.
	const float u2_; //!< First UV coordinate associated to p2_.
	const float v2_; //!< Second UV coordinate associated to p2_.
	const float u3_; //!< First UV coordinate associated to p3_.
	const float v3_; //!< Second UV coordinate associated to p3_.

	/**
	 * \fn Vector BarycenticCoordinates(const Point &p) const
	 * \brief Computes the barycentric coordinates of the input point.
	 * \param p Point assumed to lie in the embedding plane of the triangle.
	 * \return A Vector \f$\left(\lambda_1, \lambda_2, \lambda_3\right)\f$ of
	 *         the barycentric coordinates of, respectively, the first, second
	 *         and third vertex.
	 */
	Vector BarycenticCoordinates(const Point &p) const;

public:
	/**
	 * \fn Triangle(const Point &p1, const Point &p2, const Point &p3, const Vector &normal1, const Vector &normal2, const Vector &normal3, const std::shared_ptr<cimg_library::CImg<unsigned char>> &diffuse_texture, const std::shared_ptr<cimg_library::CImg<unsigned char>> &specular_texture, bool has_uv_coordinates, float u1, float v1, float u2, float v2, float u3, float v3, const Material &material=Material{})
	 * \brief Complete constructor of Triangle.
	 * \param p1, p2, p3 Vertices of the triangle.
	 * \param normal1, normal2, normal3 Normals corresponding to each vertex of
	 *        the triangle.
	 * \param diffuse_texture Pointer to a potential diffuse texture.
	 * \param specular_texture Pointer to a potential specular texture.
	 * \param has_uv_coordinates Indicates if the input UV coordinates are
	 *        valid.
	 * \param u1, v1, u2, v2, u3, v3 UV coordinates corresponding to each vertex
	 *        of the triangle.
	 */
	Triangle(
		const Point &p1,
		const Point &p2,
		const Point &p3,
		const Vector &normal1,
		const Vector &normal2,
		const Vector &normal3,
		const std::shared_ptr<cimg_library::CImg<unsigned char>> &
			diffuse_texture,
		const std::shared_ptr<cimg_library::CImg<unsigned char>> &
			specular_texture,
		bool has_uv_coordinates,
		float u1,
		float v1,
		float u2,
		float v2,
		float u3,
		float v3,
		const Material &material=Material{}
	) :
		RawObject{material, true},
		p1_{p1},
		p2_{p2},
		p3_{p3},
		normal_plane_{(p2-p1)^(p3-p1)},
		normal1_{normal1},
		normal2_{normal2},
		normal3_{normal3},
		diffuse_texture_{diffuse_texture},
		specular_texture_{specular_texture},
		has_uv_coordinates_{has_uv_coordinates},
		u1_{u1},
		v1_{v1},
		u2_{u2},
		v2_{v2},
		u3_{u3},
		v3_{v3}
	{
		normal1_.Normalize();
		normal2_.Normalize();
		normal3_.Normalize();
		normal_plane_.Normalize();
		if ((normal_plane_ | normal1) < 0) {
			normal_plane_ = -normal_plane_;
		}
	}

	/// Indicates if this triangle is associated to a diffuse texture.
	inline bool HasDiffuseTexture() const {
		return static_cast<bool>(diffuse_texture_);
	}

	/// Indicates if this triangle is associated to a specular texture.
	inline bool HasSpecularTexture() const {
		return static_cast<bool>(specular_texture_);
	}

	Intersection Intersect(const Ray &r) const;

	/**
	 * \fn Vector Normal(const Point &p) const
	 * \brief Compute the normal at the given point using its barycentric
	 *        coordinates.
	 * \note The input point should contain the barycentric coordinates
 	 *       corresponding to this Triangle.
	 *
	 * Smooths the normals of the triangle by computing them using a combination
	 * of the normals of the triangle's vertices, weighted by the barycentic
	 * coordinates.
	 */
	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;

	/**
	 * \fn Vector DiffuseColor(const Point &p) const
	 * \brief Computes the diffuse color of the triangle at a given point.
	 * \note The input point should contain the barycentric coordinates
 	 *       corresponding to this Triangle.
	 */
	Vector DiffuseColor(const Point &p) const;

	/**
	 * \fn Vector SpecularColor(const Point &p) const
	 * \brief Computes the specular color of the triangle at a given point.
	 * \note The input point should contain the barycentric coordinates
 	 *       corresponding to this Triangle.
	 */
	Vector SpecularColor(const Point &p) const;
};


/**
 * \class AABB
 * \brief Axis-Aligned Bounding Box object, defined by two extremal points.
 * \warning Should not be used in an actual scene; designed for BVH heuristics.
 */
class AABB : public RawObject {
private:
	Point p1_; //!< First extremal point.
	Point p2_; //!< Second extremal point.

public:
	/// Creates an empty box.
	AABB() :
	 	RawObject{Material{}, false}
	{
	}

	/// Creates a Box from two extremal points.
	AABB(
		const Point &p1,
		const Point &p2
	) :
		RawObject{Material{}, false},
		p1_{p1},
		p2_{p2}
	{
	}

	/// Outputs the minimum and maximum x of the box.
	inline std::pair<double, double> XMinMax() const {
		if (p1_.x() < p2_.x()) {
			return {p1_.x(), p2_.x()};
		} else {
			return {p2_.x(), p1_.x()};
		}
	}

	/// Outputs the minimum and maximum y of the box.
	inline std::pair<double, double> YMinMax() const {
		if (p1_.y() < p2_.y()) {
			return {p1_.y(), p2_.y()};
		} else {
			return {p2_.y(), p1_.y()};
		}
	}

	/// Outputs the minimum and maximum z of the box.
	inline std::pair<double, double> ZMinMax() const {
		if (p1_.z() < p2_.z()) {
			return {p1_.z(), p2_.z()};
		} else {
			return {p2_.z(), p1_.z()};
		}
	}

	/// Outputs the centroid of the box.
	inline Point Centroid() const {
		return Point(
			(p1_.x() + p2_.x())/2,
			(p1_.y() + p2_.y())/2,
			(p1_.z() + p2_.z())/2
		);
	}

	Intersection Intersect(const Ray &r) const;

	/// \warning Does not return the normal of the object. Should not be used.
	Vector Normal(const Point &p) const;

	inline AABB BoundingBox() const {
		return AABB{*this};
	}

	/// Bounding box of two AABBs.
	AABB operator||(const AABB &aabb) const;
};


/**
 * \class Object
 * \brief Class representing a wrapper of a RawObject
 *
 * An Object contains an unknown RawObject and acts as an interface for usual
 * methods that can be called on objects.
 */
class Object {
private:
	std::shared_ptr<RawObject> raw_object_; //!< RawObject contained in the Object.

public:
	/// Creates an empty / invisible object.
	Object() :
		raw_object_{new Sphere(Sphere(-1, Point(0, 0, 0), Material{}))}
	{
	}

	/// Creates an object from a Sphere.
	Object(const Sphere &s) :
		raw_object_{new Sphere{s}}
	{
	}

	/// Creates an object from a Plane.
	Object(const Plane &plane) :
		raw_object_{new Plane{plane}}
	{
	}

	/// Creates an object from a Triangle.
	Object(const Triangle &triangle) :
		raw_object_{new Triangle{triangle}}
	{
	}

	/**
	 * \fn Object(Mesh &mesh)
	 * \brief Creates an object from a Mesh.
	 * \warning Empties the input mesh.
	 */
	Object(Mesh &mesh);

	/// Creates an object from an AABB.
	Object(const AABB &aabb) :
		raw_object_{new AABB{aabb}}
	{
	}

	/// Outputs the Material of the object.
	inline const Material& ObjectMaterial() const {
		return raw_object_->ObjectMaterial();
	}

	/// Indicates if the object has null volume (if is not empty).
	inline bool IsFlat() const {
		return raw_object_->IsFlat();
	}

	/// Intersection primitive of the contained object.
	inline Intersection Intersect(const Ray &r) const {
		return raw_object_->Intersect(r);
	}

	/// Computes the normalized normal vector to the object at the given point.
	inline Vector Normal(const Vector &p) const {
		return raw_object_->Normal(p);
	}

	/// Outputs a bounding box of the object as an AABB.
	inline AABB BoundingBox() const {
		return raw_object_->BoundingBox();
	}
};
