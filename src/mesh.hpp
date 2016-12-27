/**
 * \file mesh.cpp
 * \brief Defines the Mesh object.
 */

#pragma once

#include "object_container.hpp"


/**
 * \class Mesh
 * \brief Defines a set of triangles using a BVH.
 */
class Mesh : public RawObject {
private:
	std::unique_ptr<BVH> triangles_; //!< BVH representing the Mesh.

	/*
	 * \fn void Import(const std::string &filename, const std::string &folder, const Material &material)
	 * \brief Loads into the Mesh the model given in the input path.
     * \param filename Path to the object file.
     * \param folder Folder of the texture files (with separator at the end).
	 * \param material Default material of the object, used when some parameter
	 *        is missing in the input file.
	 *
	 * Loads a model stored in the given file using library Assimp. Supports
	 * .obj format when the normals are specified, and maybe some others (to be
	 * determined).
	 *
	 * Loads textures in the file if there are some; otherwise, uses the
	 * diffuse color of the input material. This method only loads main textures
	 * (first texture of each stack).
	 *
	 * Loads part of the material of the mesh:
	 *  - diffuse color;
	 *  - specular color;
	 *  - transparent color;
	 *  - opacity;
	 *  - specular coefficient;
	 *  - specular fraction;
	 *  - refractive index.
	 * Other parameters are taken in the input material of this method.
	 *
	 * Credits to Maverick Chardet for half of the code of this function.
	 */
	void Import(
		const std::string &filename, const std::string &folder,
		const Material &material
	);

public:
    /**
     * \fn Mesh(const std::string &filename, const std::string &folder, const Material &material=Material{}
     * \brief Builds a mesh from a respresentation stored in a file.
     * \param filename Path to the object file.
     * \param folder Folder of the texture files (with separator at the end).
	 * \param material Default material of the object, used when some parameter
	 *        is missing in the input file.
     */
    Mesh(
		const std::string &filename,
		const std::string &folder,
		const Material &material=Material{}
	) :
        RawObject{material, false}
    {
        Import(filename, folder, material);
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

	inline Intersection Intersect(const Ray &r) const {
		return triangles_->Intersect(r);
	}

	/// \warning Does not return the normal of the object. Normals to individual
	///          triangles should be used instead.
	Vector Normal(const Point &p) const;

	AABB BoundingBox() const;
};
