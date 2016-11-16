#include "scene.hpp"


int main(int argc, char **argv) {
	Material green = Material(Vector(0,0.7,0.2));
	Material blue = Material(Vector(0.3,0.1,0.8));
	Material red = Material(Vector(0.9,0.1,0));
	Material white = Material(Vector(1,1,1));
	Material glass = Material(Vector(0.9,0.1,0), 0, 0, true, 1.33);
	Material red_refract = Material(Vector(0.9,0.1,0), 0, 0, true, 1.8);
	Material red_mirror = Material(Vector(0.9,0.1,0), 0);
	Object o1 = Object(Sphere(1, Point(4, 0, 0)), red_refract);
	Object o2 = Object(Plane(Point(0,3,0), Vector(0,3,0)), red);
	Object o3 = Object(Plane(Point(0,-3,0), Vector(0,3,0)), blue);
	Object o4 = Object(Plane(Point(0,0,3), Vector(0,0,2)), green);
	Object o5 = Object(Plane(Point(0,0,-1.5), Vector(0,0,2)), blue);
	Object o6 = Object(Plane(Point(9,0,0), Vector(1,0,0)), red);
	Object o7 = Object(Plane(Point(-3,0,0), Vector(1,0,0)), blue);
	std::vector<Object> objects_v = {o1, o2, o3, o4, o5, o6, o7/*, glass1, glass2*/};
	ObjectVector objects(objects_v.begin(), objects_v.end());
	Camera camera(Point(0,0,0), Vector(1,0,0), Vector(0,0,1), 60*PI/180, 900, 1600);
	Scene scene(camera, objects);
	scene.AddLight(Light(Point(2, -2, 2), Vector(40, 40, 40)));
	scene.Render(15, 100, true);
	scene.Save("test.bmp");
	return 0;
}
