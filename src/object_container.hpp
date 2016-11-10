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
	const Object empty_object_ = Object();

public:
	/**
	 * \fn virtual std::pair<Intersection, const Object&> Intersect(const Ray &r) const
	 * \brief Computes the closest Intersection with the input Ray to the origin
	 *        of this Ray, and returns also the corresponding object.
	 */
	virtual std::pair<Intersection, const Object&> Intersect(const Ray &r) const
		= 0;
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
	ObjectVector(InputIterator first, InputIterator last)
		: objects_(first, last)
	{
	}

	std::pair<Intersection, const Object&> Intersect(const Ray &r) const;
};
