/**
 * \file utils.hpp
 * \brief Defines useful classes used in the rest of the program (Vector, Ray,
 *        Intersection).
 */

#pragma once

#include <iostream>
#include <cmath>


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

public:
	/// Initiates a Vector as the origin \f$\left(0,0,0\right)\f$.
	Vector() : Vector{0, 0, 0}
	{
	}

	/// Initiates a Vector from the given coordinates.
	Vector(double x, double y, double z) : x_{x}, y_{y}, z_{z}
	{
	}

	/// Returns the first coordinate of the Vector.
	double x() const {
		return x_;
	}

	/// Returns the second coordinate of the Vector.
	double y() const {
		return y_;
	}

	/// Returns the third coordinate of the Vector.
	double z() const {
		return z_;
	}

	/// Normalizes the Vector with a unitary norm.
	void Normalize() {
		double norm = Norm();
		x_ = x_/norm;
		y_ = y_/norm;
		z_ = z_/norm;
	}

	/// Returns the squared norm of the Vector.
	double NormSquared() const {
		return x_*x_ + y_*y_ + z_*z_;
	}

	/// Returns the norm of the Vector.
	double Norm() const {
		return sqrt(NormSquared());
	}

	/// Left-multiplication of a Vector by a scalar.
	friend Vector operator*(double lambda, const Vector &v) {
		return Vector(lambda*v.x(), lambda*v.y(), lambda*v.z());
	}

	/// Right-multiplication of a Vector by a scalar.
	Vector operator*(double lambda) const {
		return Vector(lambda*x_, lambda*y_, lambda*z_);
	}

	/// Division of a Vector by a scalar.
	Vector operator/(double lambda) const {
		return Vector(x_/lambda, y_/lambda, z_/lambda);
	}

	/// Opposite of a Vector.
	Vector operator-() const {
		return Vector(-x_, -y_, -z_);
	}

	/// Addition of two Vectors.
	Vector operator+(const Vector &v) const {
		return Vector(x_+v.x(), y_+v.y(), z_+v.z());
	}

	/// Substraction of two Vectors.
	Vector operator-(const Vector &v) const {
		return Vector(x_-v.x(), y_-v.y(), z_-v.z());
	}

	/// Dot product between two Vectors.
	double operator|(const Vector &v) const {
		return x_*v.x() + y_*v.y() + z_*v.z();
	}

	/// Cross product with another Vector.
	Vector operator^(const Vector &v) const {
		return Vector(y_*v.z()-z_*v.y(), z_*v.x()-x_*v.z(), x_*v.y()-y_*v.x());
	}

	/// Outputs the Vector.
	friend std::ostream & operator<<(std::ostream &out, const Vector &v) {
		out << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
		return out;
	}
};


/**
 * \class Ray
 * \brief Represents a ray, i.e. a half-line defined by its origin and a
 *        direction.
 */
class Ray {
private:
	const Vector origin_; //!< Source point of the Ray.
	Vector direction_;    //!< Direction of the Ray; assumed to be normalized.

public:
	/// Constructs a Ray from its origin and a direction.
	Ray(const Vector &origin, const Vector &direction) :
		origin_{origin}, direction_{direction}
	{
		direction_.Normalize(); // The direction is directly normalized.
	}

	/// Returns the source of the Ray.
	const Vector& Origin() const {
		return origin_;
	}

	/// Returns the normalized direction of the Ray.
	const Vector& Direction() const {
		return direction_;
	}

	/**
	 * \fn Vector operator()(double t) const
	 * \brief Gives the point on the Ray at a given distance of the origin.
	 *
	 * A small espilon is substracted from the given distance to get a point
	 * that is "before" the intersection.
	 */
	Vector operator()(double t) const {
		return origin_ + (t*0.999999)*direction_;
	}
};


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

public:
	/// Creates an empty Intersection.
	Intersection() : exists_{false}
	{
	}

	/**
	 * \fn Intersection(double t)
	 * \brief Creates an Intersection using the given ray parameter.
	 * \param t Ray parameter at which the ray reached the intersection point,
	 *          i.e. the latter is at distance t from the origin, following the
	 *          ray direction.
	 *
	 * If the input paramter is non-positive, then the Intersection is empty.
	 */
	Intersection(double t) : exists_{t > 0}, t_{std::max(t, 0.)}
	{
	}

	/// Indicates if the Intersection is empty.
	bool IsEmpty() const {
		return !exists_;
	}

	/**
	 * \fn double Distance() const
	 * \brief Outputs the ray parameter corresponding to the intersection point.
	 * \remark Has no relevance if the Intersection is empty.
	 */
	double Distance() const {
		return t_;
	}

	/**
	 * \fn Intersection operator|(const Intersection &inter) const
	 * \brief Join operator on Intersections.
	 *
	 * Chooses the closest Intersection to the origin point.
	 */
	Intersection operator|(const Intersection &inter) const {
		if (IsEmpty()) {
			return Intersection(inter.Distance());
		} else if (inter.IsEmpty()) {
			return Intersection(Distance());
		} else {
			return Intersection(std::min(Distance(), inter.Distance()));
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
