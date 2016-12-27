/**
 * \file utils.hpp
 * \brief Defines useful classes used in the rest of the program (Vector, Ray,
 *        Intersection).
 */

#pragma once

#include <functional>
#include <iostream>
#include <iomanip>
#include <cmath>


const double PI = 3.14159265358979323846;


void show_progress(double progress);


/**
 * \class Vector
 * \brief Defines a simple class representing vectors of \f$\mathbb{R}^3\f$.
 *
 * A Vector can be used either as a point or as a usual vector.
 */
class Vector {
private:
	double x_; //!< First coordinate.
	double y_; //!< Second coordinate.
	double z_; //!< Third coordinate.

	/// First barycentric coordinate corresponding to this point's triangle, if
	/// relevant.
	double b1_ = 1;
	double b2_ = 0; //!< Second barycentric coordinate.
	double b3_ = 0; //!< Third barycentric coordinate.

public:
	/// Initiates a Vector as the origin \f$\left(0,0,0\right)\f$.
	Vector() :
		Vector{0, 0, 0}
	{
	}

	/// Initiates a Vector from the given coordinates.
	Vector(double x, double y, double z) :
		x_{x},
		y_{y},
		z_{z}
	{
	}

	/// Initiates a Point from another point and a triplet of barycentric
	/// coordinates.
	Vector(const Vector &p, const Vector &barycentric) :
		x_{p.x()},
		y_{p.y()},
		z_{p.z()},
		b1_{barycentric.x()},
		b2_{barycentric.y()},
		b3_{barycentric.z()}
	{
	}

	/// Returns the first coordinate of the Vector.
	inline const double& x() const {
		return x_;
	}

	/// Returns the second coordinate of the Vector.
	inline const double& y() const {
		return y_;
	}

	/// Returns the third coordinate of the Vector.
	inline const double& z() const {
		return z_;
	}

	/// Returns the first barycentric coordinate of the Point.
	inline const double& b1() const {
		return b1_;
	}

	/// Returns the second barycentric coordinate of the Point.
	inline const double& b2() const {
		return b2_;
	}

	/// Returns the third barycentric coordinate of the Point.
	inline const double& b3() const {
		return b3_;
	}

	/// Normalizes the Vector with a unitary norm.
	void Normalize() {
		double norm = Norm();
		x_ = x_/norm;
		y_ = y_/norm;
		z_ = z_/norm;
	}

	/// Returns the squared norm of the Vector.
	inline double NormSquared() const {
		return x_*x_ + y_*y_ + z_*z_;
	}

	/// Returns the norm of the Vector.
	inline double Norm() const {
		return sqrt(NormSquared());
	}

	/// Outputs a normalized orthogonal vector to the Vector.
	Vector Orthogonal() const {
		if (x_ != 0 || y_ != 0) {
			Vector result(y_, -x_, 0);
			result.Normalize();
			return result;
		} else {
			return Vector(1, 0, 0);
		}
	}

	/// Left-multiplication of a Vector by a scalar.
	friend Vector operator*(double lambda, const Vector &v) {
		return Vector{lambda*v.x(), lambda*v.y(), lambda*v.z()};
	}

	/// Right-multiplication of a Vector by a scalar.
	Vector operator*(double lambda) const {
		return Vector{lambda*x_, lambda*y_, lambda*z_};
	}

	/// Division of a Vector by a scalar.
	Vector operator/(double lambda) const {
		return Vector{x_/lambda, y_/lambda, z_/lambda};
	}

	/// Opposite of a Vector.
	Vector operator-() const {
		return Vector{-x_, -y_, -z_};
	}

	/// Addition of two Vectors.
	Vector operator+(const Vector &v) const {
		return Vector{x_+v.x(), y_+v.y(), z_+v.z()};
	}

	/// Substraction of two Vectors.
	Vector operator-(const Vector &v) const {
		return Vector{x_-v.x(), y_-v.y(), z_-v.z()};
	}

	/// Multiplication component by component.
	Vector operator*(const Vector &v) const {
		return Vector{x_*v.x(), y_*v.y(), z_*v.z()};
	}

	/// Dot product between two Vectors.
	double operator|(const Vector &v) const {
		return x_*v.x() + y_*v.y() + z_*v.z();
	}

	/// Cross product with another Vector.
	Vector operator^(const Vector &v) const {
		return Vector{y_*v.z()-z_*v.y(), z_*v.x()-x_*v.z(), x_*v.y()-y_*v.x()};
	}

	/// Outputs the Vector.
	friend std::ostream & operator<<(std::ostream &out, const Vector &v) {
		out << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
		return out;
	}
};


/// A Vector can be seen as a point.
typedef Vector Point;


/**
 * \class Ray
 * \brief Represents a ray, i.e. a half-line defined by its origin and a
 *        direction.
 */
class Ray {
private:
	const Point origin_; //!< Source point of the Ray.
	Vector direction_;   //!< Direction of the Ray; assumed to be normalized.

public:
	/// Constructs a Ray from its origin and a direction.
	Ray(const Point &origin, const Vector &direction) :
		origin_{origin},
		direction_{direction}
	{
		direction_.Normalize(); // The direction is directly normalized.
	}

	/// Returns the source of the Ray.
	inline const Point& Origin() const {
		return origin_;
	}

	/// Returns the normalized direction of the Ray.
	inline const Vector& Direction() const {
		return direction_;
	}

	/**
	 * \fn Point operator()(double t) const
	 * \brief Gives the point on the Ray at a given distance of the origin.
	 *
	 * A small espilon is substracted from the given distance to get a point
	 * that is "before" the intersection.
	 */
	inline Point operator()(double t) const {
		return origin_ + (t*0.999999)*direction_;
	}
};


class RawObject;

/**
 * \class Intersection
 * \brief Represents an intersection point, or the empty set.
 */
class Intersection {
private:
	bool exists_; //!< Indicates if there is an intersection.

	/// Distance on the ray from its origin for which the intersection point is
	/// reached. Assumed to be non-negative.
	double t_;

	bool out_; //!< Indicates if the intersection happens out of the object.

	/// Barycentric coordinates corresponding to the intersection point's
	/// triangle, if relevant.
	Vector barycentric_; //!< Barycentric

	/// Object corresponding to the intersection.
	std::reference_wrapper<const RawObject> object_;

public:
	/// Creates an empty Intersection.
	Intersection(std::reference_wrapper<const RawObject> object) :
		exists_{false},
		t_{0},
		out_{false},
		object_{object}
	{
	}

	/**
	 * \fn Intersection(double t, bool out, std::reference_wrapper<const RawObject> object)
	 * \brief Creates an Intersection using the given ray parameter.
	 * \param t Ray parameter at which the ray reached the intersection point,
	 *          i.e. the latter is at distance t from the origin, following the
	 *          ray direction.
	 * \param out Indicates if the intersection happens out of the object.
	 * \param object Object to which the intersection is tested.
	 *
	 * If the input parameter is non-positive, then the Intersection is empty.
	 */
	Intersection(double t, bool out,
		std::reference_wrapper<const RawObject> object
	) :
		exists_{t > 0},
		t_{std::max(t, 0.)},
		out_{out},
		object_{object}
	{
	}

	/// Builds an Intersection using the barycentric coordinates of the
	/// intersection point.
	Intersection(double t, bool out, const Vector &barycentric,
		std::reference_wrapper<const RawObject> object
	) :
		exists_{t > 0},
		t_{std::max(t, 0.)},
		out_{out},
		barycentric_{barycentric},
		object_{object}
	{
	}

	/// Indicates if the Intersection is empty.
	inline bool IsEmpty() const {
		return !exists_;
	}

	/**
	 * \fn double Distance() const
	 * \brief Outputs the ray parameter corresponding to the intersection point.
	 * \remark Has no relevance if the Intersection is empty.
	 */
	inline double Distance() const {
		return t_;
	}

	/// Indicates if the intersection happens out of the object.
	inline bool IsOut() const {
		return out_;
	}

	/// Returns the barycentric coordinates associated to the intersection
	/// point.
	inline const Vector& BarycentricCoordinates() const {
		return barycentric_;
	}

	/// Outputs the object corresponding to the Intersection.
	inline std::reference_wrapper<const RawObject> Object() const {
		return object_;
	}

	/**
	 * \fn Intersection operator|(const Intersection &inter) const
	 * \brief Join operator on Intersections.
	 *
	 * Chooses the closest Intersection to the origin point.
	 */
	Intersection operator|(const Intersection &inter) const {
		if (IsEmpty()) {
			return Intersection{
				inter.Distance(), inter.IsOut(), inter.BarycentricCoordinates(),
				inter.Object()};
		} else if (inter.IsEmpty()) {
			return Intersection{t_, out_, barycentric_, object_};
		} else {
			if (t_ < inter.Distance()) {
				return Intersection{t_, out_, barycentric_, object_};
			} else {
				return Intersection{
					inter.Distance(), inter.IsOut(),
					inter.BarycentricCoordinates(), inter.Object()};
			}
		}
	}

	/// Compares two Intersections with respect to their distance to the origin
	/// point of a Ray.
	bool operator<(const Intersection &inter) const {
		if (IsEmpty()) {
			return false;
		} else if (inter.IsEmpty()) {
			return true;
		} else {
			return Distance() < inter.Distance();
		}
	}
};
