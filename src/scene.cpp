/**
 * \file scene.cpp
 * \brief Implements classes of scene.hpp.
 */

#include "scene.hpp"


Ray Camera::Launch(int i, int j, double di, double dj) {
	Vector ray_direction =
		(j + dj - (double)width_/2 + 0.5)*right_
		+ (i + di - (double)height_/2 + 0.5)*up_
		+ height_/(2*tan(fov_/2))*direction_;
	return Ray(origin_, ray_direction);
}
