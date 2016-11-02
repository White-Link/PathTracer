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


void Scene::Render() {
	#pragma omp parallel for schedule(dynamic, 1)
	for (size_t i=0; i<Height(); i++){
		for (size_t j=0; j<Width(); j++) {
			Ray r = camera_.Launch(i, j);
			Intersection inter = objects_->Intersect(r).first;
			if (!inter.IsEmpty()) {
				image_.at((Height()-i-1)*Width()+j) = 255;
				image_.at((Height()-i-1)*Width()+j + Width()*Height()) = 255;
				image_.at((Height()-i-1)*Width()+j + 2*Width()*Height()) = 255;
			}
		}
	}
}

void Scene::Save(const std::string &file_name) const {
	cimg_library::CImg<unsigned char> cimg(
		image_.data(), Width(), Height(), 1, 3);
	cimg.save(file_name.data());
}
