/**
 * \file mesh.cpp
 * \brief Defines the Mesh object.
 */

#pragma once

#include "object_container.hpp"


/**
 * \class Mesh
 * \brief Defines a set of triangles (geometrical model).
 */
class Mesh : public RawObject {
private:
	/// Set of triangles representing the Mesh.
	std::unique_ptr<BVH> triangles_;

	/*
	 * \fn void Import(const std::string &filename, bool smooth_normals, const Material &material)
	 * \brief Loads into the Mesh the model given in the input path.
	 * \param material Material of the object.
	 * \param smooth_normals Indicates if the normals of the input model should
	 *        be smoothed.
	 *
	 * Loads a model stored in the given file using Assimp library. Supports
	 * .obj format when the normals are specified, and maybe some others (to be
	 * determined).
	 *
	 * Credits to Maverick Chardet for half of the code of this function.
	 */
	void Import(const std::string &filename, const Material &material);

public:
    /**
     * \fn Mesh(const std::string &filename, bool smooth_normals, const Material &material=Material())
     * \brief Builds a mesh from a respresentation stored in a file.
     * \param filename Path to the object file.
	 * \param smooth_normals Indicates if the normals of the input model should
	 *        be smoothed.
     */
    Mesh(const std::string &filename, const Material &material=Material()
	) :
        RawObject{material, false}
    {
        Import(filename, material);
    }

	/**
	 * \fn Mesh(Mesh &mesh)
	 * \brief Copy constructor of the input Mesh.
	 * \warning Empties the input Mesh.
	 */
	Mesh(Mesh &mesh) :
        RawObject{mesh.material_, false}
	{
		std::vector<Object> dummy;
		triangles_ = std::make_unique<BVH>(dummy.begin(), dummy.end());
		triangles_.swap(mesh.triangles_);
	}

	Intersection Intersect(const Ray &r) const;

	/// \warning Does not return the normal of the object. Normals to individual
	///          triangles should be used instead.
	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;
};
