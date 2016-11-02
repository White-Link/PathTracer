#include "scene.hpp"


int main(int argc, char **argv) {
	Object o = Object(Sphere(1, Vector(5, 0, 0)));
	std::vector<Object> objects_v = {o};
	ObjectVector objects(objects_v.begin(), objects_v.end());
	Camera camera(Vector(0,0,0), Vector(1,0,0), Vector(0,0,1), 60*M_PI/180, 500, 500);
	Scene scene(camera, objects);
	scene.Render();
	scene.Save("test.bmp");
	return 0;
}
