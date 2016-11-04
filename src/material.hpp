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

	/// Specular (R,G,B) of the Material. TODO
	const Vector color_specular_ = Vector(0, 0, 0);

	/// Fraction of the light that is returned as diffuse color by the Material.
	/// Assumed to lie between 0 and 1.
	const double fraction_diffuse_ = 1;

	/// Indicates if there is refraction on the Material.
	const bool refractive_ = false;

	/// Refractive index of the Material.
	const double index_ = 1.33;

public:
	/// Constructs a black Material.
	Material() {}

	/// Constructs a Material from its diffuse and specular colors, and its
	/// characteristics regarding refraction and reflexion.
	Material(const Vector &color_diffuse,
		double fraction_diffuse = 1, bool refractive=false,
		double refractive_index=1.33,
		const Vector &color_specular = Vector(0, 0, 0)) :
		color_diffuse_{color_diffuse}, color_specular_{color_specular},
		fraction_diffuse_{fraction_diffuse}, refractive_{refractive},
		index_{refractive_index}
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

	/// Outputs the fraction of the light that is returned as diffuse color by
	/// the Material.
	double FractionDiffuse() const {
		return fraction_diffuse_;
	}

	/// Indicates if there is refraction on the Material.
	bool Refraction() const {
		return refractive_;
	}

	/// Outputs the refractive index of the Material.
	double RefractiveIndex() const {
		return index_;
	}
};
