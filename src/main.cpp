#include "scene.hpp"


int main(int argc, char **argv) {
	Material green = Material(Vector(0,0.7,0.2));
	Material blue = Material(Vector(0.3,0.1,0.8));
	Material red = Material(Vector(0.9,0.1,0));
	Material red_refract = Material(Vector(0.9,0.1,0), 0, true, 1.8);
	Material red_mirror = Material(Vector(0.9,0.1,0), 0.2);
	Object o1 = Object(Sphere(1.4, Vector(5, 0, 0)), red_refract);
	Object o2 = Object(Plane(Vector(0,3,0), Vector(0,3,0)), red_mirror);
	Object o3 = Object(Plane(Vector(0,-3,0), Vector(0,3,0)), blue);
	Object o4 = Object(Plane(Vector(0,0,3), Vector(0,0,2)), green);
	Object o5 = Object(Plane(Vector(0,0,-1.5), Vector(0,0,2)), blue);
	Object o6 = Object(Plane(Vector(9,0,0), Vector(1,0,0)));
	Object o7 = Object(Plane(Vector(-3,0,0), Vector(1,0,0)));
	std::vector<Object> objects_v = {o1, o2, o3, o4, o5, o6, o7};
	ObjectVector objects(objects_v.begin(), objects_v.end());
	Camera camera(Vector(0,0,0), Vector(1,0,0), Vector(0,0,1), 60*M_PI/180, 900, 1600);
	Scene scene(camera, objects);
	scene.AddLight(Light(Vector(6, -1, 2.5), 30));
	scene.AddLight(Light(Vector(-2, 1, 2.5), 25));
	scene.Render(8);
	scene.Save("test.bmp");
	return 0;
}
