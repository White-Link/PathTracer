/**
 * \file material.hpp
 * \brief Defines class Material.
 */

#include "utils.hpp"


/**
 * \class Material
 * \brief Represents the material of an object (e.g. its color).
 */
class Material {
private:
	/// (R,G,B) of the Material, each component being between 0 and 1.
	const Vector color_ = Vector(1, 1, 1);

public:
	/// Constructs a black Material.
	Material() {}

	/// Constructs a Material from its color.
	Material(const Vector &color) : color_{color}
	{
	}

	const Vector& Color() const {
		return color_;
	}
};
