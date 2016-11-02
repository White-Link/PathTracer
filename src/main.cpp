#include "scene.hpp"


int main(int argc, char **argv) {
	Material green = Material(Vector(0,0.7,0.2));
	Material blue = Material(Vector(0.3,0.1,0.8));
	Material red = Material(Vector(0.9,0.1,0));
	Object o1 = Object(Sphere(0.75, Vector(5, 0, 0)), red);
	Object o2 = Object(Plane(Vector(0,3,0), Vector(0,3,0)), blue);
	Object o3 = Object(Plane(Vector(0,-3,0), Vector(0,3,0)), blue);
	Object o4 = Object(Plane(Vector(0,0,3), Vector(0,0,2)), green);
	Object o5 = Object(Plane(Vector(0,0,-1.5), Vector(0,0,2)));
	Object o6 = Object(Plane(Vector(6,0,0), Vector(1,0,0)));
	std::vector<Object> objects_v = {o1, o2, o3, o4, o5, o6};
	ObjectVector objects(objects_v.begin(), objects_v.end());
	Camera camera(Vector(0,0,0), Vector(1,0,0), Vector(0,0,1), 60*M_PI/180, 500, 800);
	Scene scene(camera, objects);
	scene.AddLight(Light(Vector(1.7, 0, 2.5), 35));
	scene.Render();
	scene.Save("test.bmp");
	return 0;
}
