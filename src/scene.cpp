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


Vector Scene::GetColor(const Ray &r) const {
	// Check first the intersectio with the objects of the scene
	std::pair<Intersection, const Object&> query = objects_->Intersect(r);
	Intersection inter = query.first;
	const Object &o = query.second;
	if (inter.IsEmpty()) {
		// No intersection
		return Vector(0, 0, 0);
	} else {
		// TODO: check when #lights != 1
		// Light intensity computation
		Vector intersection_point = r(inter.Distance());
		Vector direction_light = lights_.at(0).Source() - intersection_point;
		// Check if an object blocks the light
		// Throw a ray to the light
		Ray to_light(intersection_point, direction_light);
		Intersection inter_light = objects_->Intersect(to_light).first;
		double d = inter_light.Distance();
		// If an object is between the light and the intersection point, then
		// the color is dark
		if (!inter_light.IsEmpty() && d*d < direction_light.NormSquared()) {
			return Vector(0, 0, 0);
		} else {
			double dd = direction_light.NormSquared();
			double intensity =
				std::max(to_light.Direction()|o.Normal(intersection_point), 0.)
				* lights_.at(0).Intensity() / (M_PI * dd);
			// The color of the object is added
			const Vector& object_color = o.ObjectMaterial().Color();
			return Vector(
				intensity*object_color.x(),
				intensity*object_color.y(),
				intensity*object_color.z()
			);
		}
	}
}


void Scene::Render() {
	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t i=0; i<Height(); i++) {
		for (size_t j=0; j<Width(); j++) {
			Ray r = camera_.Launch(i, j);
			Vector color_pixel = GetColor(r);
			image_.at((Height()-i-1)*Width()+j)
				= std::min(255, (int)(255*color_pixel.x()));
			image_.at((Height()-i-1)*Width()+j + Width()*Height())
				= std::min(255, (int)(255*color_pixel.y()));
			image_.at((Height()-i-1)*Width()+j + 2*Width()*Height())
				= std::min(255, (int)(255*color_pixel.z()));
		}
	}
}

void Scene::Save(const std::string &file_name) const {
	cimg_library::CImg<unsigned char> cimg(
		image_.data(), Width(), Height(), 1, 3);
	cimg.save(file_name.data());
}
