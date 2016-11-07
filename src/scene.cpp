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


double Scene::LightIntensity(const Vector &p, const Vector &normal,
	const Light &light) const
{
	Vector direction_light = light.Source() - p;
	// Check if an object blocks the light
	// Throw a ray towards the light
	Ray to_light(p, direction_light);
	Intersection inter_light = objects_->Intersect(to_light).first;
	double d = inter_light.Distance();
	// If an object is between the light and the intersection point,
	// then the color is dark
	if (inter_light.IsEmpty() || d*d >= direction_light.NormSquared()) {
		double dd = direction_light.NormSquared();
		double intensity =
			std::max(to_light.Direction()|normal, 0.) * light.Intensity()
				/ (PI * dd);
		return intensity;
	} else {
		return 0;
	}
}


Vector Scene::GetBRDFColor(unsigned int nb_samples, unsigned int nb_recursions,
	double fraction_diffuse, double fraction_diffuse_brdf, const Vector &normal,
	const Vector &intersection_point, double index)
{
	Vector result;
	for (unsigned int i=0; i<nb_samples; i++) {
		Vector color_brdf;
		Vector ortho1 = normal.Orthogonal();
		Vector ortho2 = normal^ortho1;
		double r1 = distrib_(engine_);
		double r2 = distrib_(engine_);
		double root = sqrt(1-r2);
		Vector random_direction = cos(2*PI*r1)*root*ortho1
			+ sin(2*PI*r1)*root*ortho2 + sqrt(r2)*normal;
		if (fraction_diffuse != 0 && fraction_diffuse_brdf != 0
			&& nb_recursions != 0) {
			color_brdf =
				GetColor(Ray(intersection_point, random_direction),
					nb_recursions-1, 1, index);
		}
		result = result + color_brdf;
	}
	return fraction_diffuse * fraction_diffuse_brdf / PI * result / nb_samples;
}


Vector Scene::GetTransmissionReflexionColor(const Ray &r, const Object &o,
	const Vector &intersection_point, const Material &material,
	const Intersection &inter, double fraction_diffuse, double index,
	const Vector &normal, unsigned int nb_samples, unsigned int nb_recursions)
{
	Vector color_refracted, color_reflected;
	const Vector &ray_dir = r.Direction();

	// Refraction
	bool is_ray_refracted = false;
	double n_in = index;
	double n_out = material.RefractiveIndex();
	if (!inter.IsOut()) {
		std::swap(n_in, n_out);
	}
	double dot_prod = (ray_dir|normal);
	if (material.Refraction()) {
		double in_square_root =
			1 - n_in/n_out*n_in/n_out*(1-dot_prod*dot_prod);
		if (in_square_root > 0) {
			is_ray_refracted = true;
			Vector refracted_direction = n_in/n_out * ray_dir
				- (n_in/n_out * dot_prod + sqrt(in_square_root))
				* normal;
			double new_index = index;
			if (inter.IsOut() && o.IsFlat()) {
				new_index = material.RefractiveIndex();
			}
			color_refracted = GetColor(
				Ray(r(inter.Distance()*1.0001), refracted_direction),
				nb_recursions-1, nb_samples, new_index);
		}
	}

	// Reflection
	Ray reflected_ray(intersection_point, ray_dir - 2*dot_prod*normal);
	color_reflected =
		GetColor(reflected_ray, nb_recursions-1, nb_samples, index);
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

	return (1-fraction_diffuse) * (coef_reflection * color_reflected
		+ (1-coef_reflection) * color_refracted);
}


Vector Scene::GetColor(const Ray &r, unsigned int nb_recursions,
	double nb_samples, double index) {
	// Check first the intersection with the objects of the scene
	std::pair<Intersection, const Object&> query = objects_->Intersect(r);
	Intersection inter = query.first;
	const Object &o = query.second;
	const Material &material = o.ObjectMaterial();
	Vector intersection_point = r(inter.Distance());
	Vector normal = o.Normal(intersection_point);

	double fraction_diffuse;
	if (nb_recursions == 0) {
		fraction_diffuse = 1;
	} else {
		fraction_diffuse = material.FractionDiffuse();
	}

	double fraction_diffuse_brdf;
	if (nb_recursions == 0) {
		fraction_diffuse_brdf = 0;
	} else {
		fraction_diffuse_brdf = material.FractionDiffuseBRDF();
	}

	if (inter.IsEmpty()) {
		// No intersection
		return Vector(0, 0, 0);
	}

	// Diffuse color
	Vector diffuse_color;
	for (const auto &l : lights_) {
		double light_intensity =
			LightIntensity(intersection_point, normal, l);
		diffuse_color = diffuse_color + (1-material.FractionDiffuseBRDF())
			* fraction_diffuse * light_intensity * material.DiffuseColor();
	}
	// Diffusion
	if (nb_recursions != 0 && fraction_diffuse != 0
		&& fraction_diffuse_brdf != 0 && nb_samples != 0)
	{
		diffuse_color = diffuse_color +
			GetBRDFColor(nb_samples, nb_recursions, fraction_diffuse,
				fraction_diffuse_brdf, normal, intersection_point, index);
	}

	// Refraction and reflection
	Vector transmission_reflexion_color;
	if (fraction_diffuse != 1) {
		transmission_reflexion_color =
			GetTransmissionReflexionColor(r, o, intersection_point, material,
				inter, fraction_diffuse, index, normal, nb_samples,
				nb_recursions);
	}

	return diffuse_color + transmission_reflexion_color;
}


void Scene::Render(unsigned int nb_recursions, unsigned int nb_samples) {
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
		}
	}
}

void Scene::Save(const std::string &file_name) const {
	cimg_library::CImg<unsigned char> cimg(
		image_.data(), Width(), Height(), 1, 3);
	cimg.save(file_name.data());
}
