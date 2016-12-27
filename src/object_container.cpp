/**
 * \file object_container.cpp
 * \brief Implements classes representing sets of objects.
 */

#include <algorithm>
#include "object_container.hpp"


Intersection ObjectVector::Intersect(const Ray &r) const {
	// Find the nearest intersection by traversing the vector of objects
	Intersection inter{empty_object_};
	for (const auto &o : objects_) {
		inter = inter | o.Intersect(r);
	}
	return inter;
}


bool BVH::CompareCentroids(int i, const AABB &o1, const AABB &o2) {
	switch (i) {
		case 0 : {
			return o1.Centroid().x() < o2.Centroid().x();
		}
		case 1 : {
			return o1.Centroid().y() < o2.Centroid().y();
		}
		case 2 : {
			return o1.Centroid().z() < o2.Centroid().z();
		}
		// Default case, for invalid coordinates.
		default : {
			return false;
		}
	}
}


Intersection BVH::Intersect(const Ray &r) const {
	if (IsLeaf()) {
		// First check bounding box
		if (!bounding_box_.Intersect(r).IsEmpty()) {
			return object_.Intersect(r);
		} else {
			return Intersection{empty_object_};
		}
	} else if (!bounding_box_.Intersect(r).IsEmpty()) {
		Intersection inter_child1 = child1_->Intersect(r);
		Intersection inter_aabb_child2 = child2_->bounding_box_.Intersect(r);
		// If the intersection with the second child's bounding box arises
		// after the intersection with the first child, then it is over
		if (inter_child1 < inter_aabb_child2) {
			return inter_child1;
		} else {
			// Otherwise, test also the second child
			auto inter_child2 = child2_->Intersect(r);
			return inter_child1 | inter_child2;
		}
	} else {
		return Intersection{empty_object_};
	}
}


void BVH::Build(
	std::vector<std::pair<Object, AABB>>::iterator first,
	std::vector<std::pair<Object, AABB>>::iterator last,
	std::default_random_engine &engine,
	std::uniform_int_distribution<int> &distrib
) {
	// If there is only one object, creates a leaf
	if (last == first + 1) {
		object_ = first->first;
		bounding_box_ = first->second;
	} else if (last > first) {
		// Otherwise, divide the set using a sort on a random coordinate and
		// iterates on the children
		auto half = first + (last-first)/2;
		int coordinate = distrib(engine);
		// A full sort is not necessary: uses separation with pivot
		std::nth_element(
			first, half, last,
			[&coordinate](
				const std::pair<Object, AABB> &o1,
				const std::pair<Object, AABB> &o2
			) {
				return CompareCentroids(coordinate, o1.second, o2.second);
			}
		);
		child1_.reset(new BVH); child1_->Build(first, half, engine, distrib);
		child2_.reset(new BVH); child2_->Build(half, last, engine, distrib);
		bounding_box_ = child1_->bounding_box_ || child2_->bounding_box_;
	}
}
