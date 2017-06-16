#include "scene.hpp"
#include "mesh.hpp"


int main(int argc, char **argv) {
	Material green = Material(
		Vector(0,0.7,0.2),
		Vector(1,1,1),
		Vector(1,1,1),
		1,
		0,
		100,
		0,
		false,
		1
	);
	Material blue = Material(
		Vector(0.3,0.1,0.8),
		Vector(1,1,1),
		Vector(1,1,1),
		1,
		0,
		100,
		0,
		false,
		1
	);
	Material red = Material(
		Vector(0.9,0.1,0),
		Vector(1,1,1),
		Vector(1,1,1),
		1,
		0,
		100,
		0,
		false,
		1
	);
	Material white = Material(
		Vector(1,1,1),
		Vector(1,1,1),
		Vector(1,1,1),
		1,
		0,
		100,
		0,
		false,
		1
	);
	Sphere o1 = Sphere(1, Point(3, 0, 0), white);
	Plane o2 = Plane(Point(0,3,0), Vector(0,3,0), red);
	Plane o3 = Plane(Point(0,-3,0), Vector(0,3,0), blue);
	Plane o4 = Plane(Point(0,0,3), Vector(0,0,2), green);
	Plane o5 = Plane(Point(0,0,-1.5), Vector(0,0,2), blue);
	Plane o6 = Plane(Point(9,0,0), Vector(1,0,0), red);
	Plane o7 = Plane(Point(-3,0,0), Vector(1,0,0), blue);
	std::vector<Object> objects_v = {o1, o2, o3, o4, o5, o6, o7};
	ObjectVector objects(objects_v.begin(), objects_v.end());
	Camera camera(Point(-1,0,0), Vector(1,0,0), Vector(0,0,1), 60*PI/180, 1000, 1000);
	Scene scene(camera, objects);
	scene.AddLight(Light(Point(-2, -1, 2), Vector(50, 50, 50)));
	scene.Render(10, 50, true, false);
	scene.Save("test.bmp");
	return 0;
}
