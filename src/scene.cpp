/**
 * \file scene.cpp
 * \brief Implements classes of scene.hpp.
 */

#include "CImg.h"
#include "scene.hpp"


Ray Camera::Launch(size_t i, size_t j, double di, double dj) {
	Vector ray_direction =
		(j + dj - (double)width_/2 + 0.5)*right_
		+ (i + di - (double)height_/2 + 0.5)*up_
		+ height_/(2*tan(fov_/2))*direction_;
	return Ray(origin_, ray_direction);
}


Vector Scene::LightIntensity(const Point &p, const Vector &normal,
	const Light &light, const Ray &r, const Material &material,
	double fraction_diffuse, double fraction_diffuse_brdf) const
{
	if (fraction_diffuse != 0 || material.FractionSpecular() != 0) {
		Vector direction_light = light.Source() - p;
		// Check if an object blocks the light
		// Throw a ray towards the light
		Ray to_light(p, direction_light);
		Intersection inter_light = objects_->Intersect(to_light).first;
		double d = inter_light.Distance();
		// If an object is between the light and the intersection point,
		// then the color is dark
		if (inter_light.IsEmpty() || d*d >= direction_light.NormSquared()) {
			Vector color_light;
			// Diffuse part
			double dd = direction_light.NormSquared();
			color_light = color_light
				+ std::max(to_light.Direction()|normal, 0.) * light.Intensity()
				* (1-fraction_diffuse_brdf) * fraction_diffuse / (PI * dd)
				* material.DiffuseColor();

			// Specular part
			direction_light.Normalize();
			Vector direction_light_reflected = direction_light
				- 2*(direction_light|normal)*normal;
			direction_light_reflected.Normalize();
			color_light = color_light + material.FractionSpecular()
				* pow(std::max(direction_light_reflected|r.Direction(), 0.),
					material.SpecularCoefficient())
				* light.Intensity() * material.SpecularColor() / (PI * dd);

			return color_light;
		}
	}
	return Vector(0, 0, 0);
}


Vector Scene::GetBRDFColor(unsigned int nb_samples, unsigned int nb_recursions,
	double fraction_diffuse, double fraction_diffuse_brdf,
	const Material &material, const Vector &normal,
	const Point &intersection_point, double index, double intensity)
{
	double coef = fraction_diffuse * fraction_diffuse_brdf / PI;
	Vector result;
	Vector ortho1 = normal.Orthogonal();
	Vector ortho2 = normal^ortho1;
	for (unsigned int i=0; i<nb_samples; i++) {
		double r1 = distrib_(engine_);
		double r2 = distrib_(engine_);
		double root = sqrt(1-r2);
		Vector random_direction = cos(2*PI*r1)*root*ortho1
			+ sin(2*PI*r1)*root*ortho2 + sqrt(r2)*normal;
		result = result +
			GetColor(Ray(intersection_point, random_direction),
				nb_recursions-1, 1, index, intensity*coef);
	}
	return coef * result / nb_samples * material.DiffuseColor();
}


Vector Scene::GetTransmissionReflexionColor(const Ray &r, const Object &o,
	const Point &intersection_point, const Material &material,
	const Intersection &inter, double fraction_diffuse, double index,
	const Vector &normal, unsigned int nb_samples, unsigned int nb_recursions,
	double intensity)
{
	Vector refracted_direction, reflected_direction;
	const Vector &ray_dir = r.Direction();

	// Refraction
	bool is_ray_refracted = false;
	double dot_prod = (ray_dir|normal);
	double n_in = index;
	double n_out = material.RefractiveIndex();
	if (!inter.IsOut()) {
		std::swap(n_in, n_out);
	}
	double new_index = index;
	if (inter.IsOut() && o.IsFlat()) {
		new_index = material.RefractiveIndex();
	}
	if (material.Refraction()) {
		double in_square_root =
			1 - n_in/n_out*n_in/n_out*(1-dot_prod*dot_prod);
		if (in_square_root > 0) {
			is_ray_refracted = true;
			refracted_direction = n_in/n_out * ray_dir
				- (n_in/n_out * dot_prod + sqrt(in_square_root)) * normal;
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
			(n_in - n_out)*(n_in - n_out)/(n_in + n_out)*(n_in + n_out);
		double c = 1+dot_prod;
		coef_reflection = 1-k0 - (1-k0)*c*c*c*c*c;
	}

	Vector final_color;
	if (coef_reflection == 1) {
		final_color = GetColor(Ray(intersection_point, reflected_direction),
			nb_recursions-1, nb_samples, index, (1-fraction_diffuse)*intensity);
	} else if (coef_reflection == 0) {
		final_color = GetColor(
			Ray(r(inter.Distance()*1.0001), refracted_direction),
			nb_recursions-1, nb_samples, new_index,
			(1-fraction_diffuse)*intensity);
	} else {
		for (unsigned int i=0; i<nb_samples; i++) {
			double p = distrib_(engine_);
			if (p <= coef_reflection) {
				final_color = final_color + GetColor(
					Ray(intersection_point, reflected_direction),
					nb_recursions-1, 1, index,
					(1-fraction_diffuse)*coef_reflection*intensity);
			} else {
				final_color = final_color + GetColor(
					Ray(r(inter.Distance()*1.0001), refracted_direction),
					nb_recursions-1, 1, new_index,
					(1-fraction_diffuse)*(1-coef_reflection)*intensity);
			}
		}
		if (nb_samples != 0) {
			final_color = final_color / nb_samples;
		}
	}

	return (1-fraction_diffuse) * final_color;
}


Vector Scene::GetColor(const Ray &r, unsigned int nb_recursions,
	unsigned int nb_samples, double index, double intensity) {
	// Check first the intersection with the objects of the scene
	std::pair<Intersection, const Object&> query = objects_->Intersect(r);
	Intersection inter = query.first;

	if (inter.IsEmpty() || intensity < 0.002) {
		// No intersection
		return Vector(0, 0, 0);
	}

	const Object &o = query.second;
	const Material &material = o.ObjectMaterial();
	Point intersection_point = r(inter.Distance());
	Vector normal = o.Normal(intersection_point);

	double fraction_diffuse;
	double fraction_diffuse_brdf;
	if (nb_recursions == 0 || nb_samples == 0) {
		fraction_diffuse = 1;
		fraction_diffuse_brdf = 0;
	} else {
		fraction_diffuse = material.FractionDiffuse();
		fraction_diffuse_brdf = material.FractionDiffuseBRDF();
	}

	// Diffuse and specular color
	Vector diffuse_color;
	for (const auto &l : lights_) {
		Vector color = LightIntensity(intersection_point, normal, l, r,
			material, fraction_diffuse, fraction_diffuse_brdf);
		diffuse_color = diffuse_color + color;
	}

	// Diffusion
	if (nb_recursions != 0 && fraction_diffuse != 0
		&& fraction_diffuse_brdf != 0 && nb_samples != 0)
	{
		diffuse_color = diffuse_color +
			GetBRDFColor(nb_samples, nb_recursions, fraction_diffuse,
				fraction_diffuse_brdf, material, normal, intersection_point,
				index, intensity);
	}

	// Refraction and reflection
	Vector transmission_reflexion_color;
	if (fraction_diffuse != 1) {
		transmission_reflexion_color =
			GetTransmissionReflexionColor(r, o, intersection_point, material,
				inter, fraction_diffuse, index, normal, nb_samples,
				nb_recursions, intensity);
	}

	return diffuse_color + transmission_reflexion_color;
}


void Scene::Render(unsigned int nb_recursions, unsigned int nb_samples,
	bool progress_bar) {
	size_t computed_pixels = 0;
	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t i=0; i<Height(); i++) {
		for (size_t j=0; j<Width(); j++) {
			Ray r = camera_.Launch(i, j);
			Vector color_pixel = GetColor(r, nb_recursions, nb_samples);
			// Gamma correction
			image_.at((Height()-i-1)*Width()+j)
				= std::min(255, (int)(255*pow(color_pixel.x(), 1/gamma_)));
			image_.at((Height()-i-1)*Width()+j + Width()*Height())
				= std::min(255, (int)(255*pow(color_pixel.y(), 1/gamma_)));
			image_.at((Height()-i-1)*Width()+j + 2*Width()*Height())
				= std::min(255, (int)(255*pow(color_pixel.z(), 1/gamma_)));

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

void Scene::Save(const std::string &file_name) const {
	cimg_library::CImg<unsigned char> cimg(
		image_.data(), Width(), Height(), 1, 3);
	cimg.save(file_name.data());
}
