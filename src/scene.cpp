/**
 * \file scene.cpp
 * \brief Implements classes of scene.hpp.
 */

#include "scene.hpp"


Ray Camera::Launch(size_t i, size_t j, double di, double dj) const {
	Vector ray_direction =
		(j + dj - (double)width_/2 + 0.5)*right_
		+ (i + di - (double)height_/2 + 0.5)*up_
		+ height_/(2*tan(fov_/2))*direction_;
	return Ray{origin_, ray_direction};
}


Vector Scene::LightIntensity(
	const Point &p, const Vector &normal, const Ray &r,
	const Material &material, const Vector &diffuse_color,
	const Vector &specular_color, double opacity, double fraction_diffuse_brdf
) const {
	if (
		opacity*(1-fraction_diffuse_brdf) == 0
		&& material.SpecularCoefficient() == 0
	) {
		return Vector{0, 0, 0};
	}

	Vector final_color;

	// Traverses the set of lights
	for (const auto &l : lights_) {
		// Throw a ray towards the light
		Vector direction_light = l.Source() - p;
		Ray to_light{p, direction_light};
		Intersection inter_light = objects_->Intersect(to_light);
		double d = inter_light.Distance();
		// If an object is between the light and the intersection point,
		// then the color is dark
		if (inter_light.IsEmpty() || d*d >= direction_light.NormSquared()) {
			Vector color_light;
			// Diffuse part
			double dd = direction_light.NormSquared();
			color_light = color_light
				+ std::max(to_light.Direction()|normal, 0.) * l.Intensity()
				* opacity*(1-fraction_diffuse_brdf) / (PI * dd)
				* diffuse_color;

			// Specular part
			if (material.FractionSpecular() != 0) {
				direction_light.Normalize();
				Vector direction_light_reflected = direction_light
					- 2*(direction_light|normal)*normal;
				direction_light_reflected.Normalize();
				color_light = color_light + material.FractionSpecular()
					* pow(std::max(direction_light_reflected|r.Direction(),
							0.),
						material.SpecularCoefficient())
					* l.Intensity() * specular_color / (PI * dd);
			}

			final_color = final_color + color_light;
		}
	}

	return final_color;
}


Vector Scene::GetBRDFColor(
	unsigned int nb_samples, unsigned int nb_recursions, double intensity,
	const Vector &diffuse_color, const Vector &normal,
	const Point &intersection_point, double index
) {
	Vector result;
	Vector ortho1 = normal.Orthogonal();
	Vector ortho2 = normal^ortho1;
	for (unsigned int i=0; i<nb_samples; i++) {
		// Launches a random ray into the half plane defined by the intersection
		// point and its normal
		double r1 = distrib_(engine_);
		double r2 = distrib_(engine_);
		double root = sqrt(1-r2);
		Vector random_direction = cos(2*PI*r1)*root*ortho1
			+ sin(2*PI*r1)*root*ortho2 + sqrt(r2)*normal;
		result = result +
			GetColor(Ray{intersection_point, random_direction},
				nb_recursions-1, 1, index, intensity);
	}
	return result / (nb_samples * PI) * diffuse_color;
}


Vector Scene::GetTransmissionReflexionColor(
	unsigned int nb_samples, unsigned int nb_recursions, double intensity,
	const Ray &r, const RawObject &o, const Point &intersection_point,
	const Material &material, const Vector &specular_color,
	const Intersection &inter, double index, const Vector &normal)
{
	Vector refracted_direction, reflected_direction;
	const Vector &ray_dir = r.Direction();

	// Refraction part
	bool is_ray_refracted = false;
	double dot_prod = (ray_dir | normal);
	// Determines the refactive index of each material
	double n_in = index;
	double n_out = material.RefractiveIndex();
	if (!inter.IsOut()) {
		std::swap(n_in, n_out);
	}
	double in_out_ = n_in/n_out;
	// Determines the new index of the ambient material
	double new_index = index;
	if (inter.IsOut() && o.IsFlat()) {
		new_index = material.RefractiveIndex();
	}
	// Determines if there is refraction
	if (material.Refraction()) {
		double in_square_root =
			1 - in_out_*in_out_*(1-dot_prod*dot_prod);
		if (in_square_root > 0) {
			is_ray_refracted = true;
			refracted_direction = in_out_ * ray_dir
				- (in_out_ * dot_prod + sqrt(in_square_root)) * normal;
			refracted_direction.Normalize();
		}
	}

	// Reflection
	reflected_direction = ray_dir - 2*dot_prod*normal;
	double coef_reflection;
	if (!is_ray_refracted) {
		coef_reflection = 1;
	} else {
		// Fresnel coefficients (approximation)
		double k0 =
			(n_in - n_out)*(n_in - n_out)/((n_in + n_out)*(n_in + n_out));
		double c = 1 + dot_prod;
		coef_reflection = k0 + (1-k0)*c*c*c*c*c;
	}

	// Samples the rays between refraction and reflection using Fresnel
	// coefficients, if the coefficients are not 0/1
	Vector final_color;
	if (coef_reflection >= 0.999) {
		final_color = specular_color *
			GetColor(
				Ray{intersection_point, reflected_direction}, nb_recursions-1,
				nb_samples, index, intensity
			)
		;
	} else if (coef_reflection <= 0.001) {
		final_color = material.TransparentColor() *
			GetColor(
				Ray{r(inter.Distance()*1.0001), refracted_direction},
				nb_recursions-1, nb_samples, new_index, intensity
			)
		;
	} else {
		for (unsigned int i=0; i<nb_samples; i++) {
			double p = distrib_(engine_);
			if (p <= coef_reflection) {
				final_color = final_color + specular_color *
					GetColor(
						Ray{intersection_point, reflected_direction},
						nb_recursions-1, 1, index, coef_reflection*intensity
					)
				;
			} else {
				final_color = final_color + material.TransparentColor()
					* GetColor(
						Ray{r(inter.Distance()*1.0001), refracted_direction},
						nb_recursions-1, 1, new_index,
						(1-coef_reflection)*intensity
					)
				;
			}
		}
		if (nb_samples != 0) {
			final_color = final_color / nb_samples;
		}
	}

	return final_color;
}


Vector Scene::GetColor(const Ray &r, unsigned int nb_recursions,
	unsigned int nb_samples, double index, double intensity) {
	// Check first the intersection with the objects of the scene
	Intersection inter = objects_->Intersect(r);

	if (inter.IsEmpty() || intensity < 0.01) {
		// No intersection, or the resulting intensity in the final image is too
		// low
		return Vector{0, 0, 0};
	}

	// Definition of parameters for the following computations
	const RawObject &o = inter.Object();
	const Material &material = o.ObjectMaterial();
	Point intersection_point = Point(r(inter.Distance()),
		inter.BarycentricCoordinates());
	Vector normal = o.Normal(intersection_point);

	double opacity;
	double fraction_diffuse_brdf;
	if (nb_recursions == 0 || nb_samples == 0) {
		opacity = 1;
		fraction_diffuse_brdf = 0;
	} else {
		opacity = material.Opacity();
		fraction_diffuse_brdf = material.FractionDiffuseBRDF();
	}

	Vector diffuse_color;
	Vector specular_color;
	if (opacity != 0) {
		diffuse_color = o.DiffuseColor(intersection_point);
	}
	if (material.FractionSpecular() != 0) {
		specular_color = o.SpecularColor(intersection_point);
	}

	// Sampling between diffusion and reflection / transmission, if one part is
	// not predominant
	Vector final_color;
	if (opacity != 1 || fraction_diffuse_brdf != 0) {
		double fraction_diffusion =
			opacity*fraction_diffuse_brdf
				/ (1 - opacity*(1-fraction_diffuse_brdf));
		if (fraction_diffusion >= 0.999) {
			final_color =
				GetBRDFColor(
					nb_samples, nb_recursions,
					opacity * fraction_diffuse_brdf * intensity, diffuse_color,
					normal, intersection_point, index
				)
			;
		} else if (fraction_diffusion <= 0.001) {
			final_color =
				GetTransmissionReflexionColor(
					nb_samples, nb_recursions, (1-opacity) * intensity, r, o,
					intersection_point, material, specular_color, inter, index,
					normal
				)
			;
		} else {
			for (unsigned int i=0; i<nb_samples; i++) {
				double p = distrib_(engine_);
				if (p <= fraction_diffusion) {
					final_color = final_color +
						GetBRDFColor(
							1, nb_recursions,
							opacity*fraction_diffuse_brdf*intensity,
							diffuse_color, normal, intersection_point, index
						)
					;
				} else {
					final_color = final_color +
						GetTransmissionReflexionColor(
							1, nb_recursions, (1-opacity)*intensity, r, o,
							intersection_point, material, specular_color, inter,
							index, normal
						)
					;
				}
			}
			if (nb_samples != 0) {
				final_color = final_color / nb_samples;
			}
		}
	}

	// Adds direct illuminations
	final_color = (1-opacity*(1-fraction_diffuse_brdf)) * final_color
		+ LightIntensity(
			intersection_point, normal, r,	material, diffuse_color,
			specular_color, opacity, fraction_diffuse_brdf
		)
	;

	return final_color;
}


void Scene::Render(unsigned int nb_recursions, unsigned int nb_samples,
	bool anti_aliasing, bool progress_bar) {
	size_t computed_pixels = 0;
	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t i=0; i<Height(); i++) {
		for (size_t j=0; j<Width(); j++) {
			Vector color_pixel;
			if (!anti_aliasing) {
				// Usual procedure without anti-aliasing: the launched ray will
				// be duplicated when needed
				Ray r = camera_.Launch(i, j);
				color_pixel = GetColor(r, nb_recursions, nb_samples);
			} else if (nb_samples != 0) {
				// For anti-aliasing, nb_samples rays are generated using a
				// Gaussian distribution centered at the center of the pixel
				for (unsigned int k=0; k<nb_samples; k++) {
					double x = distrib_(engine_);
					double y = distrib_(engine_);
					double R = sqrt(-2*log(x));
					double di = R*cos(2*PI*y)*0.5;
					double dj = R*sin(2*PI*y)*0.5;
					Ray r = camera_.Launch(i, j, di, dj);
					color_pixel = color_pixel
						+ GetColor(r, nb_recursions, 1);
				}
				color_pixel = color_pixel / nb_samples;
			}

			// Gamma correction and image storage
			image_.at((Height()-i-1)*Width()+j)
				= std::min(255, (int)(255*pow(color_pixel.x(), 1/gamma_)));
			image_.at((Height()-i-1)*Width()+j + Width()*Height())
				= std::min(255, (int)(255*pow(color_pixel.y(), 1/gamma_)));
			image_.at((Height()-i-1)*Width()+j + 2*Width()*Height())
				= std::min(255, (int)(255*pow(color_pixel.z(), 1/gamma_)));

			// Prints progress bar if needed
			if (progress_bar) {
				#pragma omp critical
				{
				computed_pixels++;
				show_progress((double) computed_pixels / (Height()*Width()));
				}
			}
		}
	}
	std::cout << std::endl;
}


void Scene::Save(const std::string &filename) const {
	cimg_library::CImg<unsigned char> cimg{
		image_.data(),
		static_cast<unsigned int>(Width()), static_cast<unsigned int>(Height()),
		1, 3
	};
	cimg.save(filename.data());
}
