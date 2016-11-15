/**
 * \file scene.hpp
 * \brief Defines classes allowing to define a scene (Camera, Light, Scene).
 */

#pragma once

#include <random>
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
	/// up and direction are supposed to be orthogonal.
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
	const Vector source_;     //!< Point which light comes from.
	const Vector intensity_ ; //!< Colored intensity of the light.

public:
	/// Constructs a Light from its punctual source and an intensity.
	Light(Vector source, const Vector &intensity) :
		source_{source}, intensity_{intensity}
	{
	}

	/// Outputs the source point of the Light.
	const Vector& Source() const {
		return source_;
	}

	/// Outputs the (R,G,B) intensity of the Light.
	Vector Intensity() const {
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
	double gamma_ = 2.2; //!< Correction to apply to the final intensity.

	std::default_random_engine engine_;
	/// Uniforma real distribution over [0,1].
	std::uniform_real_distribution<double> distrib_
		= std::uniform_real_distribution<double>(0, 1);

	/// Computes the intensity of the light at a given point, given a normal to
	/// this point.
	Vector LightIntensity(const Vector &p, const Vector &normal,
		const Light &light, const Ray &r, const Material &material,
		double fraction_diffuse, double fraction_diffuse_brdf) const;

	/// Computes the fraction of the color that is due to diffusion of light
	/// accross the Scene, using parameters computed in GetColor.
	Vector GetBRDFColor(unsigned int nb_samples, unsigned int nb_recursions,
		double fraction_diffuse, double fraction_diffuse_brdf,
		const Material &material, const Vector &normal,
		const Vector &intersection_point, double index, double intensity);

	/// Computes the fraction of the color that is due reflection or refraction.
	Vector GetTransmissionReflexionColor(const Ray &r, const Object &o,
		const Vector &intersection_point, const Material &material,
		const Intersection &inter, double fraction_diffuse, double index,
		const Vector &normal, unsigned int nb_samples,
		unsigned int nb_recursions, double intensity);

	/**
	 * \fn Vector GetColor(const Ray &r, unsigned int nb_recursions, unsigned int nb_samples=1, double index=1, double intensity=1)
	 * \brief Computes the color (R,G,B) of the input Ray, with R, G and B
	 *        between 0 and 1.
	 * \param nb_recursions Limits the depth of the recursive calls tree.
	 * \param nb_samples Number of rays to relaunch is the diffusion
	 *        computation.
	 * \param index Refractive index of the current environment (irrelevant if
	 *        the Ray is casted from inside an object).
	 * \param intensity Importance of the computed color in the final pixel of
	 *        the original launch Ray.
	 *
	 * If a component of the color vector goes over 1, it will be counted as 1.
	 * Only complutes the diffuse part if the insity is not high enough, to
	 * prevent useless recursions.
	 * Irrelevant if nb_samples = 0.
	 */
	Vector GetColor(const Ray &r, unsigned int nb_recursions,
		unsigned int nb_samples=1, double index=1, double intensity=1);

public:
	/// Constructs a Scene from a Camera and an ObjectVector
	Scene(const Camera &camera, const ObjectVector &objects) :
		camera_{camera}, objects_{new ObjectVector(objects)}
	{
		std::random_device r;
		engine_ = std::default_random_engine(r());
		image_.assign(3*camera.Height()*camera.Width(), 0);
	}

	/// Adds the input Light to the scene.
	void AddLight(const Light &light) {
		lights_.push_back(light);
	}

	/// Sets the gamma correction.
	void SetGamma(double gamma) {
		gamma_ = gamma;
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

	/**
	 * \fn void Render(unsigned int nb_recursions, unsigned int nb_samples)
	 * \brief Renders the current scene and stores it in image_.
	 * \param nb_recursions Limits the depth of the recursive calls tree.
	 * \param nb_samples Number of rays lauched by pixel.
	 * \param progress_bar it set to true, enables a progress bar in the command
	 *        line.
	 */
	void Render(unsigned int nb_recursions, unsigned int nb_samples,
		bool progress_bar=false);

	/// Save the rendered scene into the given filename.
	void Save(const std::string &file_name) const;
};
