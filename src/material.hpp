/**
 * \file material.hpp
 * \brief Defines class Material.
 */

#include "utils.hpp"


/**
 * \class Material
 * \brief Represents the material of an object (e.g. its color, refractive
 *        index...).
 */
class Material {
private:
	/// Diffuse (R,G,B) of the Material, each component being between 0 and 1.
	Vector color_diffuse_ = Vector{1, 1, 1};

	/// Specular (R,G,B) of the Material.
	Vector color_specular_ = Vector{1, 1, 1};

	/// Transparent (R,G,B) of the Material (filters the color due to
	/// refraction).
	Vector color_transparent_ = Vector{1, 1, 1};

	/// Fraction of the light that is returned as diffuse color by the Material.
	/// Assumed to lie between 0 and 1.
	double opacity_ = 1;

	/// Fraction of the diffuse light coming from indirect illumination.
	/// Assumed to lie between 0 and 1.
	double fraction_diffuse_brdf_ = 0.5;

	/// Specular coefficient of the Material.
	double specular_coefficient_ = 30;

	/// Ponderation of the specular color. Assumed to lie between 0 and 1.
	double fraction_specular_ = 1;

	/// Indicates if there is refraction on the Material.
	bool refractive_ = true;

	/// Refractive index of the Material.
	double index_ = 1;

public:
	/// Constructs a Material from the set of its defining fields.
	Material(
		const Vector &color_diffuse = Vector{1, 1, 1},
		const Vector &color_specular = Vector{1, 1, 1},
		const Vector &color_transparent = Vector{1, 1, 1},
		double opacity=1,
		double fraction_diffuse_brdf=0.5,
		double specular_coefficient=30,
		double fraction_specular=1,
		bool refractive=true,
		double refractive_index=1
	) :
		color_diffuse_{color_diffuse},
		color_specular_{color_specular},
		color_transparent_{color_transparent},
		opacity_{opacity},
		fraction_diffuse_brdf_{fraction_diffuse_brdf},
		specular_coefficient_{specular_coefficient},
		fraction_specular_{fraction_specular},
		refractive_{refractive},
		index_{refractive_index}
	{
	}

	/// Outputs the diffuse color of the Material.
	inline const Vector& DiffuseColor() const {
		return color_diffuse_;
	}

	/// Outputs the specular color of the Material.
	inline const Vector& SpecularColor() const {
		return color_specular_;
	}

	/// Outputs the transparent color of the Material.
	inline const Vector& TransparentColor() const {
		return color_transparent_;
	}

	/// Outputs the specular coefficient of the Material.
	inline double SpecularCoefficient() const {
		return specular_coefficient_;
	}

	/// Outputs the ponderation of the specular color of the Material.
	inline double FractionSpecular() const {
		return fraction_specular_;
	}

	/// Outputs the fraction of the light that is returned as diffuse color by
	/// the Material.
	inline double Opacity() const {
		return opacity_;
	}

	/// Outputs the fraction of the diffuse light coming from reflection.
	inline double FractionDiffuseBRDF() const {
		return fraction_diffuse_brdf_;
	}

	/// Indicates if there is refraction on the Material.
	inline bool Refraction() const {
		return refractive_;
	}

	/// Outputs the refractive index of the Material.
	inline double RefractiveIndex() const {
		return index_;
	}
};
