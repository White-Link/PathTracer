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
	const Light &light) const {
	const Vector direction_light = light.Source() - p;
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
			std::max(to_light.Direction()|normal, 0.)
			* light.Intensity() / (M_PI * dd);
		return intensity;
	} else {
		return 0;
	}
}


Vector Scene::GetColor(const Ray &r, unsigned int nb_recursions,
	double nb_samples, double index) {
	// Check first the intersection with the objects of the scene
	std::pair<Intersection, const Object&> query = objects_->Intersect(r);
	Intersection inter = query.first;
	const Object &o = query.second;
	if (inter.IsEmpty()) {
		// No intersection
		return Vector(0, 0, 0);
	}

	// Light intensity computation
	Vector intersection_point = r(inter.Distance());
	Vector normal = o.Normal(intersection_point);
	Vector final_color;
	const Material &material = o.ObjectMaterial();
	double fraction_diffuse;
	if (nb_recursions == 0) {
		fraction_diffuse = 1;
	} else {
		fraction_diffuse = material.FractionDiffuse();
	}

	// Diffuse color
	for (const auto &l : lights_) {
		double light_intensity =
			LightIntensity(intersection_point, normal, l);
		final_color = final_color + (1-material.FractionDiffuseBRDF())
			* fraction_diffuse * light_intensity * material.DiffuseColor();
	}

	Vector mean_color;
	for (unsigned int i=0; i<nb_samples; i++) {
		// Diffuse BRDF
		Vector color_brdf;
		Vector ortho1 = normal.Orthogonal();
		Vector ortho2 = normal^ortho1;
		double r1 = distrib_(engine_);
		double r2 = distrib_(engine_);
		double root = sqrt(1-r2);
		Vector random_direction = cos(2*M_PI*r1)*root*ortho1
			+ sin(2*M_PI*r1)*root*ortho2 + sqrt(r2)*normal;
		if (fraction_diffuse != 0 && material.FractionDiffuseBRDF() != 0
			&& nb_recursions != 0) {
			color_brdf =
				GetColor(Ray(intersection_point, random_direction),
					nb_recursions-1, 1, index);
		}
		mean_color = mean_color
			+ fraction_diffuse * material.FractionDiffuseBRDF() / M_PI
			* color_brdf;

		// Refraction and reflexion
		if (fraction_diffuse != 1) {
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
						nb_recursions-1, 1, new_index);
				}
			}

			// Reflection
			Ray reflected_ray(intersection_point,
				ray_dir - 2*(ray_dir | normal)*normal);
			color_reflected =
				GetColor(reflected_ray, nb_recursions-1, 1, index);

			double coef_reflexion;
			if (!is_ray_refracted) {
				coef_reflexion = 1;
			} else {
				// Fresnel coefficients
				double square_root =
					sqrt(1 - n_in/n_out*n_in/n_out*(1-dot_prod*dot_prod));
				double coef_reflexion1 =
					(-n_in*dot_prod - n_out*square_root)
					/ (-n_in*dot_prod + n_out*square_root);
				coef_reflexion1 *= coef_reflexion1;
				double coef_reflexion2 =
					(-n_out*dot_prod - n_in*square_root)
					/ (-n_out*dot_prod + n_in*square_root);
				coef_reflexion2 *= coef_reflexion2;
				coef_reflexion = (coef_reflexion1 + coef_reflexion2)/2;
			}

			mean_color = mean_color + (1-fraction_diffuse)
			 	* (coef_reflexion * color_reflected
				+ (1-coef_reflexion) * color_refracted);
		}
	}
	if (nb_samples != 0) {
		mean_color = mean_color / nb_samples;
	}
	final_color = final_color + mean_color;
	return final_color;
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
