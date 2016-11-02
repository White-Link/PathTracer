/**
 * \file scene.hpp
 * \brief Defines classes allowing to define a scene (Camera, Light, Scene).
 */

#pragma once

#include "object.hpp"
#include "object_container.hpp"


/**
 * \class Camera
 * \brief Class representing a camera, with its origin, its direction, its FOV,
 *        and the dimensions of the final image. Can launch rays.
 */
class Camera {
private:
	Vector origin_;    //!< Origin of all the rays.
	Vector direction_; //!< Direction of the Camera; assumed to be normalized.
	Vector up_;        //!< Vertical of the Camera; assumed to be normalized.
	double fov_;       //!< Field of vision (in radians).
	size_t height_;    //!< Height of the final image.
	size_t width_;     //!< Width of the final image.

	/// Points to the right of the Camera; assumed to be normalized.
	Vector right_;

public:
	/// Constructs a Camera from all its defining characteritics but right_.
	Camera(const Vector &origin, const Vector &direction, const Vector &up,
		double fov, size_t height, size_t width) :
		origin_{origin}, direction_{direction}, up_{up}, fov_{fov},
		height_{height}, width_{width}
	{
		direction_.Normalize();
		up_.Normalize();
		right_ = up_^direction_;
	}

	/// Outputs the height of the final image.
	size_t Height() const {
		return height_;
	}

	/// Outputs the width of the final image.
	size_t Width() const {
		return width_;
	}

	/**
	 * \fn Ray Launch(int i, int j, double di=0, double dj=0)
	 * \brief Launches a Ray at a given of the Camera.
	 * \param i Height coordinate of the pixel.
	 * \param j Width coordinate of the pixel.
	 * \param di Perturbation of the height coordinate of the pixel.
	 * \param dj Perturbation of the width coordinate of the pixel.
	 */
	Ray Launch(size_t i, size_t j, double di=0, double dj=0);
};


/**
 * \class Light
 * \brief Represents a punctual source of light.
 */
class Light {
private:
	Vector source_;     //!< Point which light comes from.
	double intensity_ ; //!< Intensity of the light.

public:
	/// Constructs a Light from its punctual source and an intensity.
	Light(Vector source, double intensity) :
		source_{source}, intensity_{intensity}
	{
	}

	/// Outputs the source point of the Light.
	const Vector& Source() const {
		return source_;
	}

	/// Outputs the intensity of the Light.
	double Intensity() const {
		return intensity_;
	}
};


/**
 * \class Scene
 * \brief Represents a scene, containing a Camera, a vector of Lights and an
 *        ObjectContainer.
 */
class Scene {
private:
	Camera camera_; //!< Camera from which the scene is seen.
	std::shared_ptr<ObjectContainer> objects_; //!< Container of all objects.
	std::vector<unsigned char> image_; //!< The rendered scene is stored there.
	std::vector<Light> lights_; //!< Stores all the light sources in the scene.

	/**
	 * \fn Vector GetColor(const Ray &r) const
	 * \brief Computes the color (R,G,B) of the input Ray, with R, G and B
	 *        between 0 and 1.
	 *
	 * If a component of the color vector goes over 1, it will be counted as 1.
	 */
	Vector GetColor(const Ray &r) const;

public:
	/// Constructs a Scene from a Camera and an ObjectVector
	Scene(const Camera &camera, const ObjectVector &objects) : camera_{camera}
	{
		objects_.reset(new ObjectVector(objects));
		image_.assign(3*camera.Height()*camera.Width(), 0);
	}

	/// Adds the input Light to the scene.
	void AddLight(const Light &light) {
		lights_.push_back(light);
	}

	/// Outputs the current camera.
	const Camera& GetCamera() const {
		return camera_;
	}

	/// Outputs the height of the final image.
	size_t Height() const {
		return camera_.Height();
	}

	/// Outputs the width of the final image.
	size_t Width() const {
		return camera_.Width();
	}

	/// Renders the current scene and stores it in image_.
	void Render();

	/// Save the rendered scene into the given filename.
	void Save(const std::string &file_name) const;
};
