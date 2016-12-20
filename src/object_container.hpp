/**
 * \file object_container.hpp
 * \brief Defines classes that deals with sets of objects.
 */

#pragma once

#include <vector>
#include "object.hpp"


/**
 * \class ObjectContainer
 * \brief Abstract class defining what methods a container of objects should
 *        implement (Intersect).
 */
class ObjectContainer {
protected:
	/// Invisible object to return when an Intersection is empty.
	const AABB empty_object_{Vector{0, 0, 0}, Vector{0, 0, 0}};

public:
	/**
	 * \fn virtual Intersection Intersect(const Ray &r) const = 0;
	 * \brief Computes the closest Intersection with the input Ray to the origin
	 *        of this Ray, and returns also the corresponding object.
	 */
	virtual Intersection Intersect(const Ray &r) const = 0;
};


/**
 * \class ObjectVector
 * \brief Stores objects using a simple vector.
 */
class ObjectVector : public ObjectContainer {
private:
	std::vector<Object> objects_; //!< Objects are stored using this vector.

public:
	/// Constructs an ObjectVector from an iterable containing objects.
	template <class InputIterator>
	ObjectVector(InputIterator first, InputIterator last) :
		objects_{first, last}
	{
	}

	Intersection Intersect(const Ray &r) const;
};


/**
 * \class BVH
 * \brief Bounding Box Hierarchy that determines intersection with objects using
 *        bounding boxes and divide-and-conquer heuristics.
 * \remark The tree is assumed to be binary (either a node is a leaf, or it has
 *         two nodes).
 */
class BVH : public ObjectContainer {
private:
	std::unique_ptr<BVH> child1_; //!< First child of the node.
	std::unique_ptr<BVH> child2_; //!< Second child of the node.
	AABB bounding_box_;           //!< Bounding box of the BVH.

	/// Object corresponding to the node. Only relevant if it is a leaf.
	Object object_;

	/**
	 * \fn static bool CompareCentroids(int i, const std::pair<Object, AABB> &o1, const std::pair<Object, AABB> &o2)
	 * \brief Indicates whether one coordinate i of the bounding box of the
	 *        first object is stricly lower than the one of the second.
	 * \param i Coordinate to compare.
	 */
	static bool CompareCentroids(int i, const std::pair<Object, AABB> &o1,
		const std::pair<Object, AABB> &o2);

public:
	/// Default constructor.
	BVH() {};

	/// Constructs a BVH from an iterable containing objects.
	template <class InputIterator>
	BVH(InputIterator first, InputIterator last) {
		using namespace std;
		vector<std::pair<Object, AABB>> objects;
		for (std::vector<Object>::iterator it=first; it!=last; it++) {
			objects.push_back({*it, it->BoundingBox()});
		}
		default_random_engine engine = default_random_engine(
			chrono::high_resolution_clock::now().time_since_epoch().count());
		uniform_int_distribution<int> distrib =
			uniform_int_distribution<int>(0, 2);
		Build(objects.begin(), objects.end(), engine, distrib);
	}

	/// Indicates if the root node is a leaf (no child).
	bool IsLeaf() const {
		return !((child1_) || (child2_));
	}

	/// Outputs the bounding box of the container.
	const AABB& BoundingBox() const {
		return bounding_box_;
	}

	Intersection Intersect(const Ray &r) const;

	/// Builds the BVH using objects between first and last.
	void Build(std::vector<std::pair<Object, AABB>>::iterator first,
		std::vector<std::pair<Object, AABB>>::iterator last,
		std::default_random_engine &engine,
		std::uniform_int_distribution<int> &distrib);
};
