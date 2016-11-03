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
	/// Diffuse (R,G,B) of the Material, each component being between 0 and 1.
	const Vector color_diffuse_ = Vector(1, 1, 1);

	/// Specular (R,G,B) of the Material.
	const Vector color_specular_ = Vector(0, 0, 0);

	/**
	 * Represents the repartition of the light for diffusion, reflection and
	 * refraction (resp. firs, second and third coordinate).
	 *
	 * It is assumed that the sum of the components is equal to 1.
	 */
	const Vector distribution_ = Vector(1, 0, 0);

public:
	/// Constructs a black Material.
	Material() {}

	/// Constructs a Material from its diffuse and specular colors, and from its
	/// distribution.
	Material(const Vector &color_diffuse,
		const Vector &distribution = Vector(1, 0, 0),
		const Vector &color_specular = Vector(0, 0, 0)) :
		color_diffuse_{color_diffuse}, color_specular_{color_specular},
		distribution_{distribution / (distribution | Vector(1, 1, 1))}
	{
	}

	/// Outputs the diffuse color of the Material.
	const Vector& DiffuseColor() const {
		return color_diffuse_;
	}

	/// Outputs the specular color of the Material.
	const Vector& SpecularColor() const {
		return color_specular_;
	}

	/// Outputs the distribution (diffuse, reflection, refraction) of the
	/// Material.
	const Vector& Distribution() const {
		return distribution_;
	}
};
