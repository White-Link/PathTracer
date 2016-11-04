#include "object_container.hpp"


std::pair<Intersection, const Object&> ObjectVector::Intersect(const Ray &r)
	const {
	// Find the nearest intersection by looking at all the objects
	Intersection inter;
	size_t index;     // Index of the Object corresponding to inter in objects
	size_t count = 0; // Position of the currently considered object
	for (const auto &o : objects_) {
		Intersection previous = inter;
		inter = inter | o.Intersect(r);
		if (inter < previous) {
			index = count;
		}
		count++;
	}
	if (inter.IsEmpty()) {
		return {inter, empty_object_};
	} else {
		return {inter, objects_.at(index)};
	}
}
