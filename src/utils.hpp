/**
 * \file utils.hpp
 * \brief Defines useful classes used in the rest of the program (Vector, Ray).
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
public:

	double x; //!< First coodinate.
	double y; //!< Second coodinate.
	double z; //!< Third coodinate.

	/// Initiates a Vector as the origin \f$\left(0,0,0\right)\f$.
	Vector() : Vector{0, 0, 0}
	{
	}

	/// Initiates a Vector from the given coordinates.
	Vector(double x, double y, double z) : x{x}, y{y}, z{z}
	{
	}

	/// Normalizes the Vector with a unitary norm.
	void Normalize() {
		double norm = Norm();
		x = x/norm;
		y = y/norm;
		z = z/norm;
	}

	/// Returns the squared norm of the Vector.
	double NormSquared() const {
		return x*x + y*y + z*z;
	}

	/// Returns the norm of the Vector.
	double Norm() const {
		return sqrt(NormSquared());
	}

	/// Left-multiplication of a Vector by a scalar.
	friend Vector operator*(double lambda, const Vector &v) {
		return Vector(lambda*v.x, lambda*v.y, lambda*v.z);
	}

	/// Right-multiplication of a Vector by a scalar.
	Vector operator*(double lambda) const {
		return Vector(lambda*x, lambda*y, lambda*z);
	}

	/// Division of a Vector by a scalar.
	Vector operator/(double lambda) const {
		return Vector(x/lambda, y/lambda, z/lambda);
	}

	/// Opposite of a Vector.
	Vector operator-() const {
		return Vector(-x, -y, -z);
	}

	/// Addition of two Vectors.
	Vector operator+(const Vector &v) const {
		return Vector(x+v.x, y+v.y, z+v.z);
	}

	/// Substraction of two Vectors.
	Vector operator-(const Vector &v) const {
		return Vector(x-v.x, y-v.y, z-v.z);
	}

	/// Dot product between two Vectors.
	double operator|(const Vector &v) const {
		return x*v.x + y*v.y + z*v.z;
	}

	/// Cross product with anotehr Vector.
	Vector operator*(const Vector &v) const {
		return Vector(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
	}

	/// Outputs the Vector.
	friend std::ostream & operator<<(std::ostream &out, const Vector &v) {
		out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return out;
	}

};


class Ray {
private:
	const Vector origin_;    //!< Source point of the Ray.
	Vector direction_; //!< Direction of the Ray; assumed to be normalized.

public:
	/// Constructs a Ray from its origin and a direction.
	Ray(const Vector &origin, const Vector &direction) :
		origin_{origin}, direction_{direction}
	{
		direction_.Normalize(); // The direction is directly normalized.
	}
};
