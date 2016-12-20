/**
 * \file mesh.cpp
 * \brief Implements methods of class Mesh.
 */

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include "mesh.hpp"


Object::Object(Mesh &mesh) :
	raw_object_{new Mesh{mesh}}
{
}


void Mesh::Import(const std::string &filename, const Material &material)
{
	Assimp::Importer importer;

	// Removes the support of points and lines (only accepts polygons)
	const int new_AI_CONFIG_PP_SBP_REMOVE =
		aiPrimitiveType_POINT |
		aiPrimitiveType_LINE
	;
	importer.SetPropertyInteger("PP_SBP_REMOVE", new_AI_CONFIG_PP_SBP_REMOVE);

	// Removes the support of useless data structures of the imported model
	// (only the mesh matters)
	int new_AI_CONFIG_PP_RVC_FLAGS =
		aiComponent_TANGENTS_AND_BITANGENTS |
		aiComponent_BONEWEIGHTS             |
		aiComponent_ANIMATIONS              |
		aiComponent_CAMERAS
	;
	//if (!smooth_normals) {
	//	new_AI_CONFIG_PP_RVC_FLAGS |= aiComponent_NORMALS;
	//}
	importer.SetPropertyInteger("PP_RVC_FLAGS", new_AI_CONFIG_PP_RVC_FLAGS);

	// Normalizes the imported mesh so that its point coordinates lies between
	// 0 and 1
	importer.SetPropertyBool("PP_PTV_NORMALIZE", true);

	// Preprocessing options
	unsigned int post_processing =
		aiProcess_FindDegenerates        | // Removes degenerate meshes
		aiProcess_FindInvalidData        | // Removes bad data like null normals
		aiProcess_GenUVCoords            | // Enforces cartesian coordinates
		aiProcess_JoinIdenticalVertices  | // Removes duplicate vertices
		aiProcess_OptimizeMeshes         | // Reduces the number of meshes
		aiProcess_PreTransformVertices   | // Removes recursive meshes
		aiProcess_RemoveComponent        | // Applies the 2nd step of this code
		aiProcess_SortByPType            | // Isolates meshed with triangles
		aiProcess_TransformUVCoords      | // Cartesian coordinates for textures
		aiProcess_Triangulate            | // Triangulates all faces of meshes
		aiProcess_ValidateDataStructure    // Post-check of the importes mesh
	;
	// Chooses whether to enforce smooth normals
	//if (smooth_normals) {
	//	post_processing |= aiProcess_GenSmoothNormals;
	//} else {
		post_processing |= aiProcess_GenNormals;
	//}

	// Finally imports the mesh
	const aiScene *scene = importer.ReadFile(filename, post_processing);
	if (!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}
	std::vector<Object> triangles;
	// Inspects all faces of all meshes
	for (unsigned int i=0; i<scene->mNumMeshes; i++) {
		const aiMesh *mesh = scene->mMeshes[i];
		for (unsigned int j=0; j<mesh->mNumFaces; j++) {
			// The considered face is a triangle
			const unsigned int p1_index = mesh->mFaces[j].mIndices[0];
			const unsigned int p2_index = mesh->mFaces[j].mIndices[1];
			const unsigned int p3_index = mesh->mFaces[j].mIndices[2];

			// Points
			Point p1{
				mesh->mVertices[p1_index][0],
				mesh->mVertices[p1_index][1],
				mesh->mVertices[p1_index][2]
			};
			Point p2{
				mesh->mVertices[p2_index][0],
				mesh->mVertices[p2_index][1],
				mesh->mVertices[p2_index][2]
			};
			Point p3{
				mesh->mVertices[p3_index][0],
				mesh->mVertices[p3_index][1],
				mesh->mVertices[p3_index][2]
			};

			// Normals
			Vector n1{
				mesh->mNormals[p1_index][0],
				mesh->mNormals[p1_index][1],
				mesh->mNormals[p1_index][2]
			};
			Vector n2{
				mesh->mNormals[p2_index][0],
				mesh->mNormals[p2_index][1],
				mesh->mNormals[p2_index][2]
			};
			Vector n3{
				mesh->mNormals[p3_index][0],
				mesh->mNormals[p3_index][1],
				mesh->mNormals[p3_index][2]
			};

			triangles.emplace_back(Triangle(p1, p2, p3, -n1, -n2, -n3, material));
		}
	}

	triangles_.reset(new BVH(triangles.begin(), triangles.end()));

	// The generated aiScene is deleted by the library
}


Intersection Mesh::Intersect(const Ray &r) const {
	return triangles_->Intersect(r);
}


Vector Mesh::Normal(const Point &p) const {
	return Vector{0, 0, 1};
}


AABB Mesh::BoundingBox() const {
	return triangles_->BoundingBox();
}
