/**
 * \file object_container.cpp
 * \brief Implements classes that deals with sets of objects.
 */


#include <algorithm>
#include "object_container.hpp"


Intersection ObjectVector::Intersect(const Ray &r)
	const
{
	// Find the nearest intersection by looking at all the objects
	Intersection inter{empty_object_};
	size_t count = 0; // Position of the currently considered object
	for (const auto &o : objects_) {
		inter = inter | o.Intersect(r);
		count++;
	}
	if (inter.IsEmpty()) {
		return inter;
	} else {
		return inter;
	}
}


bool BVH::CompareCentroids(int i, const std::pair<Object, AABB> &o1,
	const std::pair<Object, AABB> &o2) {
	switch (i) {
		case 0 : {
			return o1.second.Centroid().x() < o2.second.Centroid().x();
		}
		case 1 : {
			return o1.second.Centroid().y() < o2.second.Centroid().y();
		}
		case 2 : {
			return o1.second.Centroid().z() < o2.second.Centroid().z();
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
		if (inter_child1 < inter_aabb_child2) {
			return inter_child1;
		} else {
			auto inter_child2 = child2_->Intersect(r);
			if (inter_child1 < inter_child2) {
				return inter_child1;
			} else {
				return inter_child2;
			}
		}
	} else {
		return Intersection{empty_object_};
	}
}


void BVH::Build(std::vector<std::pair<Object, AABB>>::iterator first,
	std::vector<std::pair<Object, AABB>>::iterator last,
	std::default_random_engine &engine,
	std::uniform_int_distribution<int> &distrib)
{
	if (last == first + 1) {
		object_ = first->first;
		bounding_box_ = first->second;
	} else if (last > first) {
		auto half = first + (last-first)/2;
		int coordinate = distrib(engine);
		std::nth_element(first, half, last,
			[&coordinate](const std::pair<Object, AABB> &o1,
				const std::pair<Object, AABB> &o2) {
					return CompareCentroids(coordinate, o1, o2);
				}
			);
		child1_.reset(new BVH); child1_->Build(first, half, engine, distrib);
		child2_.reset(new BVH); child2_->Build(half, last, engine, distrib);
		bounding_box_ = child1_->bounding_box_ || child2_->bounding_box_;
	}
}
