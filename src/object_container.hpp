/**
 * \file object_container.hpp
 * \brief Defines classes representing sets of objects.
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
	 * \fn virtual Intersection Intersect(const Ray &r) const = 0
	 * \brief Computes the closest Intersection with the input Ray to the origin
	 *        of this Ray.
	 */
	virtual Intersection Intersect(const Ray &r) const = 0;
};


/**
 * \class ObjectVector
 * \brief Stores objects using a simple vector.
 */
class ObjectVector : public ObjectContainer {
private:
	std::vector<Object> objects_; //!< vector used to store objects.

public:
	/// Constructs an ObjectVector from an iterable containing objects.
	template <class InputIterator>
	ObjectVector(
		InputIterator first,
		InputIterator last
	) :
		objects_{first, last}
	{
	}

	Intersection Intersect(const Ray &r) const;
};


/**
 * \class BVH
 * \brief Bounding Volume Hierarchy determining intersection with objects using
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
	 * \fn static bool CompareCentroids(int i, const AABB &o1, const AABB &o2)
	 * \brief Compares AABBs using their centroid.
	 * \param i Coordinate to compare.
	 *
	 * Indicates whether coordinate i of the first bounding box's centroid
	 * is stricly lower than the one of the second.
	 */
	static bool CompareCentroids(int i, const AABB &o1, const AABB &o2);

public:
	/// Default constructor.
	BVH() {};

	/// Constructs a BVH from an iterable containing objects.
	template <class InputIterator>
	BVH(
		InputIterator first,
		InputIterator last
	) {
		using namespace std;

		// Temporary vector containing the objects to store and their AABB
		vector<pair<Object, AABB>> objects;
		for (vector<Object>::iterator it=first; it!=last; it++) {
			objects.push_back({*it, it->BoundingBox()});
		}

		// Random initialization (uniform distribution over {0, 1, 2})
		default_random_engine engine = default_random_engine(
			chrono::high_resolution_clock::now().time_since_epoch().count()
		);
		uniform_int_distribution<int> distrib{0, 2};

		// Builds the BVH
		Build(objects.begin(), objects.end(), engine, distrib);
	}

	/// Indicates if the root node is a leaf.
	inline bool IsLeaf() const {
		return !((child1_) || (child2_));
	}

	/// Outputs the bounding box of the container.
	inline const AABB& BoundingBox() const {
		return bounding_box_;
	}

	/**
	 * \fn Intersection Intersect(const Ray &r) const
	 * \brief Tests the intersection of the input ray with the set of objects in
	 *        the BVH.
	 *
	 * This method first tests the intersection of the ray with the bounding box
	 * of the BVH. If it is empty, then the resulting intersection is also
	 * empty.
	 *
	 * Otherwise, if the BVH is a leaf, it returns the intersection with its
	 * associated object.
	 *
	 * Finally, if the BVH is not a leaf, this method computes the intersection
	 * with its first child. If this intersection arises before the intersection
	 * with its second child's bounding box, then it is the final intersection;
	 * otherwise, it also tests the intersection with its second child and
	 * outputs the closest one.
	 */
	Intersection Intersect(const Ray &r) const;

	/**
	 * \fn void Build(std::vector<std::pair<Object, AABB>>::iterator first, std::vector<std::pair<Object, AABB>>::iterator last, std::default_random_engine &engine, std::uniform_int_distribution<int> &distrib)
	 * \brief Builds the BVH.
	 * \param first, last Iterators delimiting the set of objects to store in
	 *        the BVH (all objects in (first, last]).
	 * \param engine, distrib Random-related parameters.
	 *
	 * If there is only one object, the method creates a leaf.
	 *
	 * Otherwise, it divides the set of objects into two parts using a pivot
	 * repartition with the median (according to a comparison on a random
 	 * coordinate), which requires linear time.
	 */
	void Build(
		std::vector<std::pair<Object, AABB>>::iterator first,
		std::vector<std::pair<Object, AABB>>::iterator last,
		std::default_random_engine &engine,
		std::uniform_int_distribution<int> &distrib
	);
};
