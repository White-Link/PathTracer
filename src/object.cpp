/**
 * \file object.cpp
 * \brief Implements classes defined in object.hpp.
 */

#include "object.hpp"


Intersection Sphere::Intersect(const Ray &r) const {
	// Equivalent to find the roots of degree 2 polynomial
	const Point &origin = r.Origin();
	double dot_prod = r.Direction() | (origin-center_);
	double delta =
		4*(dot_prod*dot_prod - (center_-origin).NormSquared() + radius_*radius_)
	;
	if (delta < 0) {
		return Intersection{*this};
	} else {
		Intersection i1 =
			Intersection{(-2*dot_prod + sqrt(delta))/2, false, *this}
		;
		Intersection i2 =
			Intersection{(-2*dot_prod - sqrt(delta))/2, true, *this};
		// The closest positive intersection is chosen
		return i1 | i2;
	}
}


Vector Sphere::Normal(const Point &p) const {
	Vector direction = p - center_;
	double distance_to_center_squared = direction.NormSquared();
	// Gives an "in" normal (directed towards the center) if p is in the sphere,
	// an "out" normal otherwise.
	direction.Normalize();
	if (distance_to_center_squared < radius_*radius_) {
		return -direction;
	} else {
		return direction;
	}
}


AABB Sphere::BoundingBox() const {
	Vector offset{radius_, radius_, radius_};
	return AABB{center_ + offset, center_ - offset};
}


Intersection Plane::Intersect(const Ray &r) const {
	const Vector &direction = r.Direction();
	double dot_prod = direction | normal_;
	if (dot_prod == 0) {
		// The ray and the plane are parallel
		return Intersection{*this};
	} else {
		return Intersection{
			-((r.Origin()-point_) | normal_) / dot_prod,
			(normal_|direction) < 0, *this
		};
	}
}


Vector Plane::Normal(const Point &p) const {
	Vector normal = normal_;
	normal.Normalize();
	// Outputs a well-oriented normal
	if (((p - point_) | normal) < 0) {
		return -normal;
	} else {
		return normal;
	}
	return normal;
}


AABB Plane::BoundingBox() const {
	// Infinite box
	double double_inf = std::numeric_limits<double>::infinity();
	Vector inf(double_inf, double_inf, double_inf);
	return AABB{-inf, inf};
}


Vector Triangle::BarycenticCoordinates(const Point &p) const {
	Vector v0 = p3_ - p1_;
	Vector v1 = p2_ - p1_;
	Vector v2 = p   - p1_;
	double dot00 = v0.NormSquared();
	double dot01 = (v0|v1);
	double dot02 = (v0|v2);
	double dot11 = v1.NormSquared();
	double dot12 = (v1|v2);
	double inv_denom = 1 / (dot00*dot11 - dot01*dot01);
	double u = (dot11*dot02 - dot01*dot12) * inv_denom;
	double v = (dot00*dot12 - dot01*dot02) * inv_denom;
	return Vector{1-u-v, v, u};
}


Intersection Triangle::Intersect(const Ray &r) const {
	const Vector &direction = r.Direction();

	// Plane intersection
	double dot_prod = (direction | normal_plane_);
	if (dot_prod == 0) {
		// The ray and the plane are parallel
		return Intersection{*this};
	} else {
		// Possible intersection
		double t = -((r.Origin()-p1_) | normal_plane_) / dot_prod;
		Vector barycentric = BarycenticCoordinates(r(t));
		if (barycentric.x() > 0 && barycentric.y() > 0 && barycentric.z() > 0) {
			return Intersection{
				t, (direction|normal_plane_) < 0, barycentric, *this
			};
		} else {
			return Intersection{*this};
		}
	}
}


Vector Triangle::Normal(const Point &p) const {
	Vector normal = p.b1()*normal1_ + p.b2()*normal2_ + p.b3()*normal3_;
	normal.Normalize();
	// Outputs a well-oriented normal
	if (((p1_-p)|normal_plane_) < 0) {
		return normal;
	} else {
		return -normal;
	}
}


AABB Triangle::BoundingBox() const {
	double x_min = std::min(p1_.x(), std::min(p2_.x(), p3_.x()));
	double x_max = std::max(p1_.x(), std::max(p2_.x(), p3_.x()));
	double y_min = std::min(p1_.y(), std::min(p2_.y(), p3_.y()));
	double y_max = std::max(p1_.y(), std::max(p2_.y(), p3_.y()));
	double z_min = std::min(p1_.z(), std::min(p2_.z(), p3_.z()));
	double z_max = std::max(p1_.z(), std::max(p2_.z(), p3_.z()));
	return AABB{Point{x_min, y_min, z_min}, Point{x_max, y_max, z_max}};
}


Vector Triangle::DiffuseColor(const Point &p) const {
	if (!HasDiffuseTexture() || !has_uv_coordinates_) {
		// If no texture can be accessed, uses the material color
		return material_.DiffuseColor();
	} else {
		// Computes the color using UV coordinates
		float u = p.b1()*u1_ + p.b2()*u2_ + p.b3()*u3_;
		float v = p.b1()*v1_ + p.b2()*v2_ + p.b3()*v3_;
		u *= diffuse_texture_->height();
		v *= diffuse_texture_->width();
		double r = (*diffuse_texture_)(u, v, 0, 0) / 256.;
		double g = (*diffuse_texture_)(u, v, 0, 1) / 256.;
		double b = (*diffuse_texture_)(u, v, 0, 2) / 256.;
		return Vector{r, g, b};
	}
}


Vector Triangle::SpecularColor(const Point &p) const {
	if (!HasSpecularTexture() || !has_uv_coordinates_) {
		// If no texture can be accessed, uses the material color
		return material_.SpecularColor();
	} else {
		// Computes the color using UV coordinates
		float u = p.b1()*u1_ + p.b2()*u2_ + p.b3()*u3_;
		float v = p.b1()*v1_ + p.b2()*v2_ + p.b3()*v3_;
		u *= specular_texture_->height();
		v *= specular_texture_->width();
		double r = (*specular_texture_)(u, v, 0, 0) / 256.;
		double g = (*specular_texture_)(u, v, 0, 1) / 256.;
		double b = (*specular_texture_)(u, v, 0, 2) / 256.;
		return Vector{r, g, b};
	}
}


Intersection AABB::Intersect(const Ray &r) const {
	Vector inv_direction = Vector{
		1/r.Direction().x(), 1/r.Direction().y(), 1/r.Direction().z()
	};
	double t_x1 = (p1_.x() - r.Origin().x())*inv_direction.x();
	double t_x2 = (p2_.x() - r.Origin().x())*inv_direction.x();
	double t_y1 = (p1_.y() - r.Origin().y())*inv_direction.y();
	double t_y2 = (p2_.y() - r.Origin().y())*inv_direction.y();
	double t_z1 = (p1_.z() - r.Origin().z())*inv_direction.z();
	double t_z2 = (p2_.z() - r.Origin().z())*inv_direction.z();
	double t_min = std::max(std::max(
		std::min(t_x1, t_x2), std::min(t_y1, t_y2)), std::min(t_z1, t_z2));
	double t_max = std::min(std::min(
		std::max(t_x1, t_x2), std::max(t_y1, t_y2)), std::max(t_z1, t_z2));
	if (t_min > t_max) {
		return Intersection{*this};
	} else {
		return Intersection{t_min, true, *this}
			| Intersection{t_max, false, *this};
	}
}


Vector AABB::Normal(const Point &p) const {
	return Vector{1, 0, 0};
}


AABB AABB::operator||(const AABB &aabb) const {
	std::pair<double, double> x_min_max_1 = XMinMax();
	std::pair<double, double> x_min_max_2 = aabb.XMinMax();
	std::pair<double, double> y_min_max_1 = YMinMax();
	std::pair<double, double> y_min_max_2 = aabb.YMinMax();
	std::pair<double, double> z_min_max_1 = ZMinMax();
	std::pair<double, double> z_min_max_2 = aabb.ZMinMax();
	return AABB{
		Point{std::min(x_min_max_1.first, x_min_max_2.first),
			std::min(y_min_max_1.first, y_min_max_2.first),
			std::min(z_min_max_1.first, z_min_max_2.first)
		},
		Point{std::max(x_min_max_1.second, x_min_max_2.second),
			std::max(y_min_max_1.second, y_min_max_2.second),
			std::max(z_min_max_1.second, z_min_max_2.second)
		}
	};
}
