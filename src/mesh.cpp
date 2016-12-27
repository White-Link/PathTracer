/**
 * \file mesh.cpp
 * \brief Implements methods of class Mesh.
 */

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/material.h>
#include "mesh.hpp"


Object::Object(Mesh &mesh) :
	raw_object_{new Mesh{mesh}}
{
}


void Mesh::Import(const std::string &filename, const std::string &folder,
	const Material &material)
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
		aiProcess_FixInfacingNormals     | // Makes normal directions coherent
		aiProcess_GenNormals             | // Generates missing normals
		aiProcess_GenUVCoords            | // Enforces UV coordinates.
		aiProcess_JoinIdenticalVertices  | // Removes duplicate vertices
		aiProcess_OptimizeMeshes         | // Reduces the number of meshes
		aiProcess_PreTransformVertices   | // Removes recursive meshes
		aiProcess_RemoveComponent        | // Applies the 2nd step of this code
		aiProcess_SortByPType            | // Isolates meshes with triangles
		aiProcess_TransformUVCoords      | // Uniformize texture transformation
		aiProcess_FlipUVs                | // Flips y-axis of textures
		aiProcess_Triangulate            | // Triangulates all faces of meshes
		aiProcess_ValidateDataStructure    // Post-check of the importes mesh
	;

	// Finally imports the mesh
	const aiScene *scene = importer.ReadFile(filename, post_processing);
	if (!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}
	std::vector<Object> triangles;
	// Inspects all faces of all meshes
	for (unsigned int i=0; i<scene->mNumMeshes; i++) {
		const aiMesh *mesh = scene->mMeshes[i];
		const aiMaterial *ai_material = scene->mMaterials[mesh->mMaterialIndex];

		// Loads diffuse texture (only the main texture)
		std::shared_ptr<cimg_library::CImg<unsigned char>> diffuse_texture;
		if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString filename;
			ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
			std::string fullpath = folder + std::string(filename.C_Str());
			diffuse_texture.reset(
				new cimg_library::CImg<unsigned char>(fullpath.c_str())
			);
		}

		// Loads specular texture (only the main texture)
		std::shared_ptr<cimg_library::CImg<unsigned char>> specular_texture;
		if (ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            aiString filename;
			ai_material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
			std::string fullpath = folder + std::string(filename.C_Str());
			specular_texture.reset(
				new cimg_library::CImg<unsigned char>(fullpath.c_str())
			);
		}

		// Builds the material of the imported model
		aiColor3D ai_color_diffuse{
			static_cast<float>(material.DiffuseColor().x()),
			static_cast<float>(material.DiffuseColor().y()),
			static_cast<float>(material.DiffuseColor().z())
		};
		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color_diffuse);
		Vector color_diffuse{
			ai_color_diffuse.r, ai_color_diffuse.g, ai_color_diffuse.b
		};
		aiColor3D ai_color_specular{
			static_cast<float>(material.SpecularColor().x()),
			static_cast<float>(material.SpecularColor().y()),
			static_cast<float>(material.SpecularColor().z())
		};;
		ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color_specular);
		Vector color_specular{
			ai_color_specular.r, ai_color_specular.g, ai_color_specular.b
		};
		aiColor3D ai_color_transparent{
			static_cast<float>(material.TransparentColor().x()),
			static_cast<float>(material.TransparentColor().y()),
			static_cast<float>(material.TransparentColor().z())
		};;
		ai_material->Get(AI_MATKEY_COLOR_TRANSPARENT, ai_color_transparent);
		Vector color_transparent{
			//ai_color_transparent.r, ai_color_transparent.g,
			1,1,1//ai_color_transparent.b
		};
		float opacity = material.Opacity();
		ai_material->Get(AI_MATKEY_OPACITY, opacity);
		float specular_coefficient = material.SpecularCoefficient();
		ai_material->Get(AI_MATKEY_SHININESS, specular_coefficient);
		float fraction_specular = material.FractionSpecular();
		ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, fraction_specular);
		float index = material.RefractiveIndex();
		ai_material->Get(AI_MATKEY_REFRACTI, index);
		Material imported_material{
			color_diffuse,
			color_specular,
			color_transparent,
			opacity,
			material.FractionDiffuseBRDF(),
			specular_coefficient,
			fraction_specular,
			material.Refraction(),
			index
		};

		for (unsigned int j=0; j<mesh->mNumFaces; j++) {
			// The considered face is a triangle
			const unsigned int id_p1 = mesh->mFaces[j].mIndices[0];
			const unsigned int id_p2 = mesh->mFaces[j].mIndices[1];
			const unsigned int id_p3 = mesh->mFaces[j].mIndices[2];

			// Points
			Point p1{
				mesh->mVertices[id_p1][0],
				mesh->mVertices[id_p1][1],
				mesh->mVertices[id_p1][2]
			};
			Point p2{
				mesh->mVertices[id_p2][0],
				mesh->mVertices[id_p2][1],
				mesh->mVertices[id_p2][2]
			};
			Point p3{
				mesh->mVertices[id_p3][0],
				mesh->mVertices[id_p3][1],
				mesh->mVertices[id_p3][2]
			};

			// Normals
			Vector n1{
				mesh->mNormals[id_p1][0],
				mesh->mNormals[id_p1][1],
				mesh->mNormals[id_p1][2]
			};
			Vector n2{
				mesh->mNormals[id_p2][0],
				mesh->mNormals[id_p2][1],
				mesh->mNormals[id_p2][2]
			};
			Vector n3{
				mesh->mNormals[id_p3][0],
				mesh->mNormals[id_p3][1],
				mesh->mNormals[id_p3][2]
			};
			n1.Normalize();
			n2.Normalize();
			n3.Normalize();

			// Face textures
			float u1, v1, u2, v2, u3, v3;
			double has_uv_coordinates = false;
			if (mesh->HasTextureCoords(0)) {
				u1 = mesh->mTextureCoords[0][id_p1].x;
				v1 = mesh->mTextureCoords[0][id_p1].y;
				u2 = mesh->mTextureCoords[0][id_p2].x;
				v2 = mesh->mTextureCoords[0][id_p2].y;
				u3 = mesh->mTextureCoords[0][id_p3].x;
				v3 = mesh->mTextureCoords[0][id_p3].y;
				if (!(u1 < 0 || u2 < 0 || u3 < 0 || v1 < 0 || v2 < 0 || v3 < 0))
				has_uv_coordinates = true;
			}
			triangles.emplace_back(
				Triangle(p1, p2, p3, n1, n2, n3, diffuse_texture,
					specular_texture, has_uv_coordinates, u1, v1, u2, v2, u3,
					v3, imported_material)
			);
		}
	}

	triangles_.reset(new BVH(triangles.begin(), triangles.end()));

	// The generated aiScene is deleted by the library
}


Vector Mesh::Normal(const Point &p) const {
	return Vector{0, 0, 1};
}


AABB Mesh::BoundingBox() const {
	return triangles_->BoundingBox();
}
