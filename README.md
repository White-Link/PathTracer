# Path Tracer (ENS Lyon -- CR11 -- Mathematical Methods for Image Synthesis -- Project)

## Author
Jean-Yves Franceschi

## Files
 - `cimg` folder: contains `cimg.h`, header file of library CImg for handling image storing.
 - `src` folder: contains the source files, with:
   - `main.cpp`: file where the tested scene is defined and the rendering step is launched;
   - `material.hpp`: defines the materials;
   - `mesh.hpp` and `mesh.cpp`: implement meshes and textures importation;
   - `object_container.hpp` and `object_container.cpp`: implement object lists and BVH;
   - `object.hpp` and `object.cpp`: implement all object types;
   - `scene.hpp` and `scene.cpp`: implement the scene;
   - `utils.hpp` and `utils.cpp`: define several useful tools for this project.
 - `examples` folder: contains several examples of main files and their corresponding result (these are the images produced for the report).
 - `car`, `lightning` and `triss` folders: contain all three models used in the examples and in the report.
 - `CMakeLists.txt`: CMake file to configure the project before compilation.
 - `Doxyfile`: configuration file for Doxygen.
 - `LICENSE`: MIT license.
 - `README.md`: this file.
 - `report.pdf`: report of this project.

## Dependencies
 - CMake v. 3.1 or above.
 - A C++ compiler supporting C++14 standard.
 - Assimp library, v. 3.3.1.
 - (Optional) Doxygen for a simple documentation of the project.

## Usage

### General usage
Given a file `main.cpp` in folder `src`, compilation is done with the following commands in a build folder:
`cmake [project root folder] && make -j` for Linux, or equivalent commands for another OS.

The executable is created is the project root folder, and is named `path_tracer`.

### Examples
In order to test one of the examples, one should copy their content to the main file in `src`, and compile again the project.

The program must be executed in the project root tree in order to work correctly (because of relative paths in the examples). However, this constraint does not hold for arbitrary executions of the program with other main files.

### Documentation
In order to produce the documentation of the code of this project, `make doc` should be executed in the build folder. It produces a `doc` folder in the root tree of the project, containing an HTML (`html/index.html`) and PDF (that should be compiled in folder `latex`) version of the documentation.
