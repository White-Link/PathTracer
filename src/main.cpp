#include "scene.hpp"
#include "mesh.hpp"


int main(int argc, char **argv) {
	/*Material green = Material(Vector(0,0.7,0.2));
	Material blue = Material(Vector(0.3,0.1,0.8));
	Material red = Material(Vector(0.9,0.1,0));
	Material white = Material(Vector(1,1,1));
	Material glass = Material(Vector(0.9,0.1,0), 0, 0, true, 1.33);
	Material red_refract = Material(Vector(0.9,0.1,0), 0, 0, true, 1.8);
	Material red_mirror = Material(Vector(0.9,0.1,0), 0);
	Object o1 = Object(Sphere(1, Point(4, 0, 0), white));
	Object o2 = Object(Plane(Point(0,3,0), Vector(0,3,0), red));
	Object o3 = Object(Plane(Point(0,-3,0), Vector(0,3,0), blue));
	Object o4 = Object(Plane(Point(0,0,3), Vector(0,0,2), green));
	Object o5 = Object(Plane(Point(0,0,-1.5), Vector(0,0,2), blue));
	Object o6 = Object(Plane(Point(9,0,0), Vector(1,0,0), red));
	Object o7 = Object(Plane(Point(-3,0,0), Vector(1,0,0), blue));
	std::vector<Object> objects_v = {o1, o2, o3, o4, o5, o6, o7};*/
	Material green = Material(Vector(0,0.7,0.2));
	Material blue = Material(Vector(0.3,0.1,0.8));
	Material red = Material(Vector(0.9,0.1,0));
	Material silver = Material(Vector(1, 1, 1), 0.5, 0.5, true, 1.5);
	Object p1 = Object(Plane(Point(0,0,-0.3), Vector(0,0,1), green));
	Object p2 = Object(Plane(Point(0,0,5), Vector(0,0,-1), green));
	Object p3 = Object(Plane(Point(0,-3,0), Vector(0,3,0), blue));
	Object p4 = Object(Plane(Point(0,3,0), Vector(0,-3,0), blue));
	Object p5 = Object(Plane(Point(-5,0,0), Vector(1,0,0), red));
	Object p6 = Object(Plane(Point(5,0,0), Vector(-1,0,0), red));
	//Mesh mesh("sls_amg.obj", silver);
	//std::vector<Object> objects_v = {mesh, p1, p2, p3, p4, p5, p6};
	//ObjectVector objects(objects_v.begin(), objects_v.end());
	//Camera camera(Point(0,0,0), Vector(1,0,0), Vector(0,0,1), 60*PI/180, 900, 1600);
	Camera camera(Point(-2,0,0), Vector(1,0,0), Vector(0,0,1), 60*PI/180, 500, 500);
	//Scene scene(camera, objects);
	//scene.AddLight(Light(Point(-3, 0, 3), Vector(30, 30, 30)));
	//scene.Render(10, 50, false, true);
	//scene.Save("test.bmp");
	return 0;
}
